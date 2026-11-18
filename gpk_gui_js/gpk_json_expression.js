(function (gpk) {
  "use strict";

  class SEvaluationStepResult {
    constructor(expression = "", output = undefined) {
      this.IndexRootJSONNode = 0;
      this.IndexJSONResult = -1;
      this.LastValue = 0;
      this.Output = output;
      this.Expression = expression;
      this.LastValueCarry = false;
    }
    SetBoolCarry(value) {
      this.IndexJSONResult = -5;
      this.LastValueCarry = true;
      this.LastValue = value ? 1 : 0;
      this.Output = !!value;
      return 0;
    }
  }

  class SJSONExpressionSolver {
    constructor() { this.Results = []; }
  }

  function jsonTruth(value) {
    if (value === null || value === undefined || value === false || value === 0 || value === "") return false;
    if (Array.isArray(value)) return value.length !== 0;
    if (typeof value === "object") return Object.keys(value).length !== 0;
    return true;
  }

  function jsonDeepEqual(left, right) {
    if (left === right) return true;
    if ((left === null || typeof left === "boolean") || (right === null || typeof right === "boolean")) return jsonTruth(left) === jsonTruth(right);
    if ((left === "" && (right === null || right === false)) || (right === "" && (left === null || left === false))) return true;
    if (typeof left === "number" && typeof right === "number") return left === right;
    if (typeof left === "string" || typeof right === "string") return false;
    if (Array.isArray(left) || Array.isArray(right)) {
      if (!Array.isArray(left) || !Array.isArray(right) || left.length !== right.length) return false;
      for (let index = 0; index < left.length; ++index) if (!jsonDeepEqual(left[index], right[index])) return false;
      return true;
    }
    if (left && right && typeof left === "object" && typeof right === "object") {
      const leftKeys = Object.keys(left), rightKeys = Object.keys(right);
      if (leftKeys.length !== rightKeys.length) return false;
      for (const key of leftKeys)
        if (!Object.prototype.hasOwnProperty.call(right, key) || !jsonDeepEqual(left[key], right[key])) return false;
      return true;
    }
    return false;
  }

  function propertyGet(value, key, kind) {
    if (value === null || value === undefined) throw new Error("Cannot access " + kind + " on null");
    if (typeof value === "string") {
      if (!Number.isInteger(Number(key)) || Number(key) < 0 || Number(key) >= value.length) throw new Error("String index out of range: " + key);
      return value[Number(key)];
    }
    if (Array.isArray(value)) {
      if (!Number.isInteger(Number(key)) || Number(key) < 0 || Number(key) >= value.length) throw new Error("Array index out of range: " + key);
      return value[Number(key)];
    }
    if (typeof value !== "object") throw new Error("Only objects can be accessed by key: " + key);
    if (!Object.prototype.hasOwnProperty.call(value, key)) throw new Error("Expression key not found: " + key);
    return value[key];
  }

  function evaluateNode(node, input, solver, source) {
    let result;
    if (node.kind === "literal") result = node.value;
    else if (node.kind === "reference") result = propertyGet(input, node.name, "key");
    else if (node.kind === "property") result = propertyGet(evaluateNode(node.object, input, solver, source), node.key, "key");
    else if (node.kind === "dynamic-key") {
      const object = evaluateNode(node.object, input, solver, source);
      const key = evaluateNode(node.key, input, solver, source);
      result = propertyGet(object, key, "key");
    }
    else if (node.kind === "index") {
      const object = evaluateNode(node.object, input, solver, source);
      const key = evaluateNode(node.key, input, solver, source);
      result = propertyGet(object, key, "index");
    }
    else if (node.kind === "group") result = evaluateNode(node.expression, input, solver, source);
    else if (node.kind === "unary") result = !jsonTruth(evaluateNode(node.operand, input, solver, source));
    else if (node.kind === "binary") {
      const left = evaluateNode(node.left, input, solver, source);
      const right = evaluateNode(node.right, input, solver, source);
      if (node.operator === "&&") result = jsonTruth(left) && jsonTruth(right);
      else if (node.operator === "||") result = jsonTruth(left) || jsonTruth(right);
      else if (node.operator === "==") result = jsonDeepEqual(left, right);
      else throw new Error("Unsupported expression operator: " + node.operator);
    }
    else if (node.kind === "conditional") {
      const condition = evaluateNode(node.condition, input, solver, source);
      result = evaluateNode(jsonTruth(condition) ? node.whenTrue : node.whenFalse, input, solver, source);
    }
    else throw new Error("Unsupported expression node: " + node.kind);

    if (solver) solver.Results.push(new SEvaluationStepResult(source.slice(node.begin, node.end), result));
    return result;
  }

  function jsonExpressionResolve(expressionOrReader, input, solver) {
    const reader = expressionOrReader instanceof gpk.SExpressionReader ? expressionOrReader : gpk.expressionParse(String(expressionOrReader));
    if (!reader.Ast) throw new Error("Expression reader has no parsed tree");
    return evaluateNode(reader.Ast, input, solver || null, reader.Source);
  }

  function jsonExpressionText(value) {
    if (value === null) return "null";
    if (value === undefined) return "";
    if (typeof value === "object") return JSON.stringify(value);
    return String(value);
  }

  function jsonStringFormat(format, input) {
    return gpk.expressionStringFormat(format, expression => jsonExpressionText(jsonExpressionResolve(expression, input)));
  }

  Object.assign(gpk, {
    SEvaluationStepResult,
    SJSONExpressionSolver,
    jsonTruth,
    jsonDeepEqual,
    jsonExpressionResolve,
    jsonExpressionText,
    jsonStringFormat
  });
})(globalThis.gpk);
