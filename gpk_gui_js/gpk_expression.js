(function (gpk) {
  "use strict";

  const EXPRESSION_READER_TYPE = Object.freeze({
    TERM_ANY: 0,
    TERM_KEY: 1,
    TERM_INDEX: 2,
    TERM_BOOL: 3,
    TERM_AND: 4,
    TERM_OR: 5,
    TERM_EQUALS: 6,
    KEY: 7,
    LITERAL: 8,
    UNARY_NOT: 9,
    CODEPOINT: 10,
    UNKNOWN: -1
  });
  const STRIP_LITERAL_TYPE = Object.freeze({LITERAL: 0, TOKEN: 1});

  class SExpressionToken {
    constructor(parentIndex = -1, type = EXPRESSION_READER_TYPE.UNKNOWN, begin = 0, end = 0, closingCondition = false) {
      this.ParentIndex = parentIndex;
      this.Type = type;
      this.Span = {Begin: begin, End: end};
      this.ClosingCondition = closingCondition;
    }
  }

  class SExpressionReaderState {
    constructor() {
      this.IndexCurrentChar = 0;
      this.IndexCurrentElement = -1;
      this.CurrentElement = null;
      this.NestLevel = 0;
      this.CharCurrent = "";
      this.Escaping = false;
      this.ExpectsSeparator = false;
      this.InsideString = false;
      this.ClosingQuotes = "";
    }
  }

  class SExpressionNode {
    constructor(token = null, parent = null, objectIndex = -1) {
      this.Token = token;
      this.Parent = parent;
      this.Children = [];
      this.ObjectIndex = objectIndex;
    }
  }

  class SExpressionReader {
    constructor() {
      this.Token = [];
      this.View = [];
      this.Tree = [];
      this.StateRead = new SExpressionReaderState();
      this.SubExpressionCache = null;
      this.Source = "";
      this.Ast = null;
      this.Lexeme = [];
    }
  }

  function isSpace(character) {
    return character === " " || character === "\t" || character === "\r" || character === "\n" || character === "\f" || character === "\b";
  }

  function decodeEscape(source, index) {
    const escaped = source[index];
    if (escaped === "b") return {value: "\b", end: index + 1};
    if (escaped === "f") return {value: "\f", end: index + 1};
    if (escaped === "n") return {value: "\n", end: index + 1};
    if (escaped === "r") return {value: "\r", end: index + 1};
    if (escaped === "t") return {value: "\t", end: index + 1};
    if (escaped === "u") {
      const digits = source.slice(index + 1, index + 5);
      if (!/^[0-9A-Fa-f]{4}$/.test(digits)) throw new Error("Invalid unicode escape at " + (index - 1));
      return {value: String.fromCharCode(parseInt(digits, 16)), end: index + 5};
    }
    return {value: escaped, end: index + 1};
  }

  function expressionTokenize(expression) {
    const source = String(expression), output = [];
    let index = 0;
    while (index < source.length) {
      const character = source[index];
      if (isSpace(character)) { ++index; continue; }
      if (character === "/" && source[index + 1] === "/") {
        index += 2;
        while (index < source.length && source[index] !== "\n") ++index;
        continue;
      }
      const pair = source.slice(index, index + 2);
      if (pair === "&&" || pair === "||" || pair === "==") {
        output.push({type: pair, value: pair, begin: index, end: index + 2, raw: pair});
        index += 2;
        continue;
      }
      if (".[]()!?:{}".includes(character) && !(character === "." && /[0-9]/.test(source[index + 1] || ""))) {
        output.push({type: character, value: character, begin: index, end: index + 1, raw: character});
        ++index;
        continue;
      }
      if (character === '"' || character === "'") {
        const begin = index, quote = character;
        let value = "";
        ++index;
        while (index < source.length && source[index] !== quote) {
          if (source[index] === "\\") {
            const decoded = decodeEscape(source, index + 1);
            value += decoded.value;
            index = decoded.end;
          }
          else {
            if (source.charCodeAt(index) < 0x20) throw new Error("Invalid control character in expression string at " + index);
            value += source[index++];
          }
        }
        if (source[index] !== quote) throw new Error("Unterminated expression string at " + begin);
        ++index;
        output.push({type: "literal", value, begin, end: index, raw: source.slice(begin, index), quoted: true});
        continue;
      }
      const numberMatch = source.slice(index).match(/^-?(?:\d+\.?(?:\d*)?|\.\d+)(?:[eE][+-]?\d+)?/);
      if (numberMatch) {
        const raw = numberMatch[0], value = Number(raw);
        output.push({type: "literal", value, begin: index, end: index + raw.length, raw, quoted: false});
        index += raw.length;
        continue;
      }
      const identifierMatch = source.slice(index).match(/^[A-Za-z_$][A-Za-z0-9_$-]*/);
      if (identifierMatch) {
        const raw = identifierMatch[0], begin = index;
        index += raw.length;
        if (raw === "true" || raw === "false" || raw === "null")
          output.push({type: "literal", value: raw === "true" ? true : raw === "false" ? false : null, begin, end: index, raw, quoted: false});
        else
          output.push({type: "identifier", value: raw, begin, end: index, raw});
        continue;
      }
      throw new Error("Unexpected expression character at " + index + ": " + character);
    }
    output.push({type: "eof", value: "", begin: source.length, end: source.length, raw: ""});
    return output;
  }

  function ast(kind, begin, end, values) {
    return Object.assign({kind, begin, end}, values || {});
  }

  class ExpressionParser {
    constructor(source) {
      this.source = String(source);
      this.tokens = expressionTokenize(this.source);
      this.index = 0;
    }
    current() { return this.tokens[this.index]; }
    accept(type) {
      if (this.current().type !== type) return null;
      return this.tokens[this.index++];
    }
    expect(type) {
      const token = this.accept(type);
      if (!token) throw new Error("Expected '" + type + "', found '" + this.current().type + "' at " + this.current().begin);
      return token;
    }
    parse() {
      if (this.current().type === "eof") throw new Error("Expression is empty");
      const result = this.conditional();
      this.expect("eof");
      return result;
    }
    conditional() {
      const condition = this.or();
      const question = this.accept("?");
      if (!question) return condition;
      const whenTrue = this.conditional();
      this.expect(":");
      const whenFalse = this.conditional();
      return ast("conditional", condition.begin, whenFalse.end, {condition, whenTrue, whenFalse});
    }
    or() {
      let result = this.and();
      for (let token; (token = this.accept("||")); ) {
        const right = this.and();
        result = ast("binary", result.begin, right.end, {operator: token.type, left: result, right});
      }
      return result;
    }
    and() {
      let result = this.equality();
      for (let token; (token = this.accept("&&")); ) {
        const right = this.equality();
        result = ast("binary", result.begin, right.end, {operator: token.type, left: result, right});
      }
      return result;
    }
    equality() {
      let result = this.unary();
      for (let token; (token = this.accept("==")); ) {
        const right = this.unary();
        result = ast("binary", result.begin, right.end, {operator: token.type, left: result, right});
      }
      return result;
    }
    unary() {
      const token = this.accept("!");
      if (!token) return this.postfix();
      const operand = this.unary();
      return ast("unary", token.begin, operand.end, {operator: token.type, operand});
    }
    postfix() {
      let result = this.primary();
      for (;;) {
        const dot = this.accept(".");
        if (dot) {
          if (this.accept("{")) {
            const key = this.conditional(), close = this.expect("}");
            result = ast("dynamic-key", result.begin, close.end, {object: result, key, separator: dot.begin});
          }
          else {
            const key = this.expect("identifier");
            result = ast("property", result.begin, key.end, {object: result, key: key.value, keyBegin: key.begin, keyEnd: key.end});
          }
          continue;
        }
        const open = this.accept("[");
        if (open) {
          const key = this.conditional(), close = this.expect("]");
          result = ast("index", result.begin, close.end, {object: result, key, open: open.begin});
          continue;
        }
        break;
      }
      return result;
    }
    primary() {
      const token = this.current();
      if (this.accept("literal")) return ast("literal", token.begin, token.end, {value: token.value, quoted: !!token.quoted});
      if (this.accept("identifier")) return ast("reference", token.begin, token.end, {name: token.value});
      const open = this.accept("(");
      if (open) {
        const expression = this.conditional(), close = this.expect(")");
        return ast("group", open.begin, close.end, {expression});
      }
      throw new Error("Expected expression value, found '" + token.type + "' at " + token.begin);
    }
  }

  function readerType(node) {
    if (node.kind === "reference" || node.kind === "property") return EXPRESSION_READER_TYPE.KEY;
    if (node.kind === "literal") return EXPRESSION_READER_TYPE.LITERAL;
    if (node.kind === "dynamic-key") return EXPRESSION_READER_TYPE.TERM_KEY;
    if (node.kind === "index") return EXPRESSION_READER_TYPE.TERM_INDEX;
    if (node.kind === "unary") return EXPRESSION_READER_TYPE.UNARY_NOT;
    if (node.kind === "conditional") return EXPRESSION_READER_TYPE.TERM_BOOL;
    if (node.kind === "binary" && node.operator === "&&") return EXPRESSION_READER_TYPE.TERM_AND;
    if (node.kind === "binary" && node.operator === "||") return EXPRESSION_READER_TYPE.TERM_OR;
    if (node.kind === "binary" && node.operator === "==") return EXPRESSION_READER_TYPE.TERM_EQUALS;
    return EXPRESSION_READER_TYPE.TERM_ANY;
  }

  function readerChildren(node) {
    if (node.kind === "property") return [node.object];
    if (node.kind === "dynamic-key" || node.kind === "index") return [node.object, node.key];
    if (node.kind === "unary") return [node.operand];
    if (node.kind === "binary") return [node.left, node.right];
    if (node.kind === "conditional") return [node.condition, node.whenTrue, node.whenFalse];
    if (node.kind === "group") return [node.expression];
    return [];
  }

  function readerView(source, node) {
    if (node.kind === "reference") return node.name;
    if (node.kind === "property") return node.key;
    if (node.kind === "literal") return typeof node.value === "string" ? node.value : String(node.value);
    return source.slice(node.begin, node.end);
  }

  function expressionReaderBuild(reader, source, parsed) {
    reader.Token.length = 0;
    reader.View.length = 0;
    reader.Tree.length = 0;
    const visit = (node, parentIndex) => {
      const index = reader.Token.length;
      const token = new SExpressionToken(parentIndex, readerType(node), node.begin, node.end, false);
      const parent = parentIndex >= 0 ? reader.Tree[parentIndex] : null;
      const treeNode = new SExpressionNode(token, parent, index);
      reader.Token.push(token);
      reader.View.push(readerView(source, node));
      reader.Tree.push(treeNode);
      if (parent) parent.Children.push(treeNode);
      for (const child of readerChildren(node)) visit(child, index);
      return index;
    };
    const rootToken = new SExpressionToken(-1, EXPRESSION_READER_TYPE.TERM_ANY, 0, source.length, false);
    const rootNode = new SExpressionNode(rootToken, null, 0);
    reader.Token.push(rootToken);
    reader.View.push(source);
    reader.Tree.push(rootNode);
    visit(parsed, 0);
  }

  function expressionReaderParse(reader, expression) {
    if (!(reader instanceof SExpressionReader)) throw new Error("expressionReaderParse requires SExpressionReader");
    const parser = new ExpressionParser(expression);
    const parsed = parser.parse();
    reader.Source = parser.source;
    reader.Ast = parsed;
    reader.Lexeme = parser.tokens.slice();
    reader.StateRead = new SExpressionReaderState();
    reader.StateRead.IndexCurrentChar = parser.source.length;
    expressionReaderBuild(reader, parser.source, parsed);
    return 0;
  }

  function expressionParse(expression) {
    const reader = new SExpressionReader();
    expressionReaderParse(reader, expression);
    return reader;
  }

  function expressionReaderParseStep(reader, expression) {
    const source = String(expression);
    if (reader.StateRead.IndexCurrentChar < 0 || reader.StateRead.IndexCurrentChar >= source.length) throw new Error("Invalid expression index: " + reader.StateRead.IndexCurrentChar);
    reader.StateRead.CharCurrent = source[reader.StateRead.IndexCurrentChar];
    if (reader.StateRead.IndexCurrentChar === source.length - 1) expressionReaderParse(reader, source);
    return 0;
  }

  function expressionStringTokens(format) {
    const source = String(format), output = [];
    let literalBegin = 0, index = 0;
    while (index < source.length) {
      if (source[index] !== "{") { ++index; continue; }
      if (index > literalBegin) output.push({Type: STRIP_LITERAL_TYPE.LITERAL, Span: {Begin: literalBegin, End: index}, Text: source.slice(literalBegin, index)});
      const tokenBegin = index++, expressionBegin = index;
      let depth = 1, quote = "", escaping = false;
      while (index < source.length && depth) {
        const character = source[index];
        if (quote) {
          if (escaping) escaping = false;
          else if (character === "\\") escaping = true;
          else if (character === quote) quote = "";
        }
        else if (character === '"' || character === "'") quote = character;
        else if (character === "{") ++depth;
        else if (character === "}") --depth;
        ++index;
      }
      if (depth) throw new Error("Unterminated format expression at " + tokenBegin);
      const tokenEnd = index;
      output.push({Type: STRIP_LITERAL_TYPE.TOKEN, Span: {Begin: tokenBegin, End: tokenEnd}, Text: source.slice(expressionBegin, tokenEnd - 1)});
      literalBegin = index;
    }
    if (literalBegin < source.length) output.push({Type: STRIP_LITERAL_TYPE.LITERAL, Span: {Begin: literalBegin, End: source.length}, Text: source.slice(literalBegin)});
    return output;
  }

  function expressionStringFormat(format, resolve) {
    let output = "";
    for (const token of expressionStringTokens(format))
      output += token.Type === STRIP_LITERAL_TYPE.TOKEN ? String(resolve(token.Text)) : token.Text;
    return output;
  }

  Object.assign(gpk, {
    EXPRESSION_READER_TYPE,
    STRIP_LITERAL_TYPE,
    SExpressionToken,
    SExpressionReaderState,
    SExpressionNode,
    SExpressionReader,
    expressionTokenize,
    expressionReaderParse,
    expressionReaderParseStep,
    expressionParse,
    expressionStringTokens,
    expressionStringFormat
  });
})(globalThis.gpk);
