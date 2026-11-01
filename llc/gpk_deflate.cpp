#include "gpk_deflate.h"
#include "gpk_array_static.h"
#include "gpk_bit.h"

GPK_USING_TYPEINT();

namespace gpk
{
		struct SDeflateMatch {
			rangeu3_t			Match					= {};
			rangeu3_t			PreviousMatch			= {};
		};

		stxp rangeu1_t	DEFLATE_MATCH_LENGTHS	= {3, 256};
		stxp u2_t		ZLIB_ADLER_MODULUS		= 65521;

		struct SZLibMethod {
			u0_t				CompressionMethod	: 4;
			u0_t				CompressionInfo		: 4;

			cxpr				SZLibMethod			(cnst SZLibMethod &)	nxpt	= default;
			cxpr				SZLibMethod			(u0_t encoded = 0)		nxpt
				: CompressionMethod	(encoded % ::gpk::bit_make<u0_t>(4))
				, CompressionInfo	(encoded / ::gpk::bit_make<u0_t>(4))
				{}
			cxpr SZLibMethod&	oper=				(cnst SZLibMethod &)			= default;
			inxp oper			u0_t				()						csnx	{ rtrn Encode(); }
			err_t				Decode				(u0_t encoded)			nxpt	{ rtrn (*this = encoded, encoded); }
			inxp u0_t			Encode				()						csnx	{
				rtrn CompressionMethod + CompressionInfo * ::gpk::bit_make<u0_t>(4);
			}
		};
		struct SZLibFlags {
			u0_t				CheckBits				: 5;
			u0_t				PresetDictionary		: 1;
			u0_t				CompressionLevel		: 2;

			cxpr				SZLibFlags			(cnst SZLibFlags &)		nxpt	= default;
			cxpr				SZLibFlags			(u0_t encoded = 0)		nxpt
				: CheckBits			(encoded % ::gpk::bit_make<u0_t>(5))
				, PresetDictionary	(encoded / ::gpk::bit_make<u0_t>(5))
				, CompressionLevel	(encoded / ::gpk::bit_make<u0_t>(6))
				{}
			cxpr SZLibFlags&	oper=				(cnst SZLibFlags &)				= default;
			inxp oper			u0_t				()						csnx	{ rtrn Encode(); }
			err_t				Decode				(u0_t encoded)			nxpt	{ rtrn (*this = encoded, encoded); }
			inxp u0_t			Encode				()						csnx	{
				rtrn CheckBits + PresetDictionary * ::gpk::bit_make<u0_t>(5) + CompressionLevel * ::gpk::bit_make<u0_t>(6);
			}
		};

		GDEFINE_ENUM_TYPE(DEFLATE_BLOCK_TYPE, u0_t);
		GDEFINE_ENUM_VALUE(DEFLATE_BLOCK_TYPE, STORED	, 0);
		GDEFINE_ENUM_VALUE(DEFLATE_BLOCK_TYPE, FIXED	, 1);
		GDEFINE_ENUM_VALUE(DEFLATE_BLOCK_TYPE, DYNAMIC	, 2);
		GDEFINE_ENUM_VALUE(DEFLATE_BLOCK_TYPE, RESERVED	, 3);

		struct SDeflateBlockHeader {
			stxp u0_t					BIT_COUNT				= 3;

			u0_t						Final				: 1;
			DEFLATE_BLOCK_TYPE			Type				: 2;

			cxpr						SDeflateBlockHeader	(cnst SDeflateBlockHeader &)	nxpt	= default;
			cxpr						SDeflateBlockHeader	(u0_t encoded = 0)				nxpt
				: Final	(encoded % ::gpk::bit_make<u0_t>(1))
				, Type	((DEFLATE_BLOCK_TYPE)(encoded / ::gpk::bit_make<u0_t>(1)))
				{}
			cxpr SDeflateBlockHeader&	oper=				(cnst SDeflateBlockHeader &)			= default;
			inxp oper					u0_t				()								csnx	{ rtrn Encode(); }
			err_t						Decode				(u0_t encoded)					nxpt	{ rtrn (*this = encoded, encoded); }
			inxp u0_t					Encode				()								csnx	{ rtrn Final + (u0_t)Type * ::gpk::bit_make<u0_t>(1); }
		};

		struct SZlibHeader {
			SZLibMethod		Method					= {};
			SZLibFlags		Flags					= {};

			inln SZlibHeader &	Set					(u0_t windowBits, u0_t compressionLevel) {
				Method.CompressionMethod	= 8;
				Method.CompressionInfo		= windowBits - 8;
				Flags.CheckBits = Flags.PresetDictionary = 0;
				Flags.CompressionLevel		= compressionLevel;
				Flags.CheckBits				= (31U - (CheckValue() % 31U)) % 31U;
				rtrn *this;
			}
			inxp u1_t		CheckValue			() csnx { rtrn ::gpk::byte_to<u1_t>(Method.Encode(), 1) + Flags.Encode(); }
			inxp u0_t		WindowBits			() csnx { rtrn Method.CompressionInfo + 8; }
			inxp bool		Valid				() csnx { rtrn 8 == Method.CompressionMethod && Method.CompressionInfo <= 7 && 0 == CheckValue() % 31U && 0 == Flags.PresetDictionary; }
		};

		struct SBitWindow {
			u3_t				Value					= 0;
			u0_t				Width					= 0;
		};
		struct SBitReader {
			vcu0_t				Remaining				= {};
			SBitWindow			Pending					= {};
		};
		struct SDeflateLink {
			u2_t				Position					= 0;
			u2_t				Previous					= 0;
		};
		struct SDeflateHistory {
			vu0_t				Window					= {};
			view<SDeflateLink>	Links					= {};
			vu2_t				Heads					= {};
		};
		struct SHuffmanTable {
			astu1_t<16>			Counts					= {};
			astu1_t<288>		Symbols					= {};
			u1_t				SymbolCount				= 0;
			u0_t				MaxWidth				= 0;
		};
		struct SAdler32 {
			u2_t				A						= 1;
			u2_t				B						= 0;
		};
		sttc err_t			zlibHeaderRead			(SZlibHeader & output, vcu0_t input) {
			if_true_fe(input.size() < 2);
			output.Method.Decode(input[0]);
			output.Flags.Decode(input[1]);
			if_true_fe(false == output.Valid());
			rtrn 0;
		}
		sttc err_t			deflateSettingsValidate	(SDeflateSettings settings) {
			if_true_fe(settings.WindowBits < 8 || settings.WindowBits > 15);
			if_true_fe(settings.CompressionLevel > 3);
			if_true_fe(settings.HashBits >= bitsof<u2_t>());
			cnst u3_t hashSize			= ::gpk::bit_make<u3_t>(settings.HashBits);
			if_true_fe(hashSize * szof(u2_t) > (u2_t)-1);
			rtrn 0;
		}

		stxp u1_t		LENGTH_BASE[]			= {3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258};
		stxp u0_t		LENGTH_EXTRA[]			= {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0};
		stxp u1_t		DISTANCE_BASE[]			= {1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577};
		stxp u0_t		DISTANCE_EXTRA[]		= {0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13};

		sttc err_t		bitsWrite				(SBitWindow & pending, au0_t & output, u3_t value, u0_t width) {
			pending.Value				|= value << pending.Width;
			pending.Width				+= width;
			while(pending.Width >= 8) {
				if_fail_fe(output.push_back((u0_t)pending.Value));
				pending.Value			>>= 8;
				pending.Width			-= 8;
			}
			rtrn 0;
		}

		sttc err_t		bitsFlush				(SBitWindow & pending, au0_t & output) {
			if(pending.Width) {
				if_fail_fe(output.push_back((u0_t)pending.Value));
			}
			pending					= {};
			rtrn 0;
		}

		sttc err_t		bitsRead				(SBitReader & input, u0_t width, u2_t & value) {
			while(input.Pending.Width < width) {
				if_true_fe(0 == input.Remaining.size());
				input.Pending.Value		|= (u3_t)input.Remaining[0] << input.Pending.Width;
				input.Pending.Width		+= 8;
				input.Remaining			= {input.Remaining.begin() + 1, input.Remaining.size() - 1};
			}
			value					= (u2_t)(input.Pending.Value & ((1ULL << width) - 1));
			input.Pending.Value		>>= width;
			input.Pending.Width		-= width;
			rtrn 0;
		}

		sttc void		bitsAlign				(SBitReader & input) {
			cnst u0_t discard		= input.Pending.Width & 7U;
			input.Pending.Value		>>= discard;
			input.Pending.Width		-= discard;
		}

		sttc err_t		fixedSymbolWrite			(SBitWindow & pending, au0_t & output, u1_t symbol) {
			u2_t code					= 0;
			u0_t width					= 0;
			if(symbol <= 143) {
				code					= 0x30U + symbol;
				width					= 8;
			}
			else if(symbol <= 255) {
				code					= 0x190U + symbol - 144U;
				width					= 9;
			}
			else if(symbol <= 279) {
				code					= symbol - 256U;
				width					= 7;
			}
			else if(symbol <= 287) {
				code					= 0xC0U + symbol - 280U;
				width					= 8;
			}
			else
				rtrn -1;
			rtrn bitsWrite(pending, output, reverse_bits(code, width), width);
		}

		sttc err_t		matchWrite				(SBitWindow & pending, au0_t & output, u2_t length, u2_t distance) {
			u2_t lengthCode				= 0;
			while(lengthCode + 1 < ::gpk::size(LENGTH_BASE) && length >= LENGTH_BASE[lengthCode + 1])
				++lengthCode;
			if_fail_fe(fixedSymbolWrite(pending, output, (u1_t)(257U + lengthCode)));
			if(LENGTH_EXTRA[lengthCode])
				if_fail_fe(bitsWrite(pending, output, length - LENGTH_BASE[lengthCode], LENGTH_EXTRA[lengthCode]));

			u2_t distanceCode			= 0;
			while(distanceCode + 1 < ::gpk::size(DISTANCE_BASE) && distance >= DISTANCE_BASE[distanceCode + 1])
				++distanceCode;
			if_fail_fe(bitsWrite(pending, output, reverse_bits(distanceCode, 5), 5));
			if(DISTANCE_EXTRA[distanceCode])
				if_fail_fe(bitsWrite(pending, output, distance - DISTANCE_BASE[distanceCode], DISTANCE_EXTRA[distanceCode]));
			rtrn 0;
		}

		sttc u2_t		matchHash				(vcu0_t input, u2_t offset, u2_t mask) {
			rtrn ((u2_t)input[offset] * 251U + (u2_t)input[offset + 1] * 17U + input[offset + 2]) & mask;
		}

		sttc SDeflateHistory deflateHistory			(SZipStream & stream) {
			rtrn
				{ stream.Window
				, {(SDeflateLink*)stream.Previous.begin(), stream.Previous.size() / szof(SDeflateLink)}
				, {(u2_t*)stream.Head.begin(), stream.Head.size() / szof(u2_t)}
				};
		}

		sttc SDeflateMatch matchFind				(SDeflateHistory & history, vcu0_t input, u2_t offset, u2_t searchDepth) {
			SDeflateMatch result			= {};
			if(offset + 2 >= input.size())
				rtrn result;
			cnst u2_t windowMask		= history.Window.size() - 1;
			cnst u2_t hash				= matchHash(input, offset, history.Heads.size() - 1);
			u2_t candidate				= history.Heads[hash];
			for(u2_t iChain = 0; candidate && iChain < searchDepth; ++iChain) {
				cnst u2_t position		= candidate - 1;
				if(position >= offset || offset - position > history.Window.size())
					break;
				cnst SDeflateLink & link	= history.Links[position & windowMask];
				if(link.Position != candidate)
					break;
				u2_t length				= 0;
				cnst u2_t available		= ::gpk::min((u2_t)(input.size() - offset), (u2_t)(DEFLATE_MATCH_LENGTHS.Offset + DEFLATE_MATCH_LENGTHS.Count - 1));
				while(length < available && input[position + length] == input[offset + length])
					++length;
				if(length > result.Match.Count) {
					result.PreviousMatch	= result.Match;
					result.Match			= {position, length};
					if(length == available)
						break;
				}
				candidate				= link.Previous;
			}
			rtrn result;
		}

		sttc void		matchInsert				(SDeflateHistory & history, vcu0_t input, u2_t offset) {
			cnst u2_t windowMask		= history.Window.size() - 1;
			history.Window[offset & windowMask] = input[offset];
			if(offset + 2 >= input.size())
				rtrn;
			cnst u2_t hash				= matchHash(input, offset, history.Heads.size() - 1);
			SDeflateLink & link			= history.Links[offset & windowMask];
			link						= {offset + 1, history.Heads[hash]};
			history.Heads[hash]			= offset + 1;
		}

		sttc void		adlerAdd				(SAdler32 & adler, u0_t value) {
			adler.A						= (adler.A + value) % ZLIB_ADLER_MODULUS;
			adler.B						= (adler.B + adler.A) % ZLIB_ADLER_MODULUS;
		}

		sttc u2_t		adlerValue				(cnst SAdler32 & adler) {
			rtrn adler.B * ::gpk::bit_make<u2_t>(16) + adler.A;
		}

		sttc err_t		huffmanBuild				(SHuffmanTable & table, vcu0_t widths, u0_t maxWidth) {
			table						= {};
			for(u2_t iSymbol = 0; iSymbol < widths.size(); ++iSymbol) {
				cnst u0_t width			= widths[iSymbol];
				if_true_fe(width > maxWidth);
				if(width) {
					++table.Counts[width];
					table.MaxWidth		= ::gpk::max(table.MaxWidth, width);
					++table.SymbolCount;
				}
			}
			if(0 == table.SymbolCount)
				rtrn 0;
			s2_t available				= 1;
			for(u0_t width = 1; width <= maxWidth; ++width) {
				available				= (available << 1) - table.Counts[width];
				if_true_fe(available < 0);
			}
			u1_t offsets[16]			= {};
			for(u0_t width = 1; width < maxWidth; ++width)
				offsets[width + 1]		= (u1_t)(offsets[width] + table.Counts[width]);
			for(u2_t iSymbol = 0; iSymbol < widths.size(); ++iSymbol)
				if(widths[iSymbol])
					table.Symbols[offsets[widths[iSymbol]]++] = (u1_t)iSymbol;
			rtrn 0;
		}

		sttc err_t		huffmanRead				(SBitReader & input, cnst SHuffmanTable & table, u1_t & symbol) {
			if_true_fe(0 == table.SymbolCount);
			u2_t code					= 0;
			u2_t first					= 0;
			u2_t index					= 0;
			for(u0_t width = 1; width <= table.MaxWidth; ++width) {
				u2_t bit				= 0;
				if_fail_fe(bitsRead(input, 1, bit));
				code					|= bit;
				cnst u2_t count			= table.Counts[width];
				if(code < first + count) {
					symbol				= table.Symbols[index + code - first];
					rtrn 0;
				}
				index					+= count;
				first					= (first + count) << 1;
				code					<<= 1;
			}
			rtrn -1;
		}

		sttc err_t		fixedTables				(SHuffmanTable & literals, SHuffmanTable & distances) {
			u0_t literalWidths[288]	= {};
			u0_t distanceWidths[32]	= {};
			for(u2_t i =   0; i <= 143; ++i) literalWidths[i] = 8;
			for(u2_t i = 144; i <= 255; ++i) literalWidths[i] = 9;
			for(u2_t i = 256; i <= 279; ++i) literalWidths[i] = 7;
			for(u2_t i = 280; i <= 287; ++i) literalWidths[i] = 8;
			for(u2_t i =   0; i < ::gpk::size(distanceWidths); ++i) distanceWidths[i] = 5;
			if_fail_fe(huffmanBuild(literals, literalWidths, 15));
			if_fail_fe(huffmanBuild(distances, distanceWidths, 15));
			rtrn 0;
		}

		sttc err_t		dynamicTables				(SBitReader & input, SHuffmanTable & literals, SHuffmanTable & distances) {
			stxp u0_t order[]			= {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};
			u2_t value					= 0;
			if_fail_fe(bitsRead(input, 5, value)); cnst u2_t literalCount	= value + 257;
			if_fail_fe(bitsRead(input, 5, value)); cnst u2_t distanceCount	= value + 1;
			if_fail_fe(bitsRead(input, 4, value)); cnst u2_t lengthCount		= value + 4;
			if_true_fe(literalCount > 286 || distanceCount > 32);

			u0_t codeWidths[19]		= {};
			for(u2_t i = 0; i < lengthCount; ++i) {
				if_fail_fe(bitsRead(input, 3, value));
				codeWidths[order[i]]		= (u0_t)value;
			}
			SHuffmanTable codeLengths	= {};
			if_fail_fe(huffmanBuild(codeLengths, codeWidths, 7));

			u0_t widths[288 + 32]		= {};
			cnst u2_t total				= literalCount + distanceCount;
			for(u2_t i = 0; i < total;) {
				u1_t symbol				= 0;
				if_fail_fe(huffmanRead(input, codeLengths, symbol));
				if(symbol <= 15) {
					widths[i++]			= (u0_t)symbol;
					continue;
				}
				u2_t repeat				= 0;
				u0_t repeatedWidth			= 0;
				if(symbol == 16) {
					if_true_fe(0 == i);
					if_fail_fe(bitsRead(input, 2, repeat)); repeat += 3;
					repeatedWidth			= widths[i - 1];
				}
				else if(symbol == 17) {
					if_fail_fe(bitsRead(input, 3, repeat)); repeat += 3;
				}
				else if(symbol == 18) {
					if_fail_fe(bitsRead(input, 7, repeat)); repeat += 11;
				}
				else
					rtrn -1;
				if_true_fe(repeat > total - i);
				while(repeat--)
					widths[i++]			= repeatedWidth;
			}
			if_true_fe(0 == widths[256]);
			if_fail_fe(huffmanBuild(literals, {widths, literalCount}, 15));
			if_fail_fe(huffmanBuild(distances, {&widths[literalCount], distanceCount}, 15));
			rtrn 0;
		}

		sttc err_t		inflateByte				(SZipStream & stream, au0_t & output, u3_t produced, SAdler32 & adler, u0_t value) {
			stream.Window[(u2_t)produced & (stream.Window.size() - 1)] = value;
			adlerAdd(adler, value);
			if_fail_fe(output.push_back(value));
			rtrn 0;
		}

		sttc err_t		inflateCompressed			(SZipStream & stream, SBitReader & input, cnst SHuffmanTable & literals, cnst SHuffmanTable & distances, au0_t & output, u3_t & produced, SAdler32 & adler) {
			while(true) {
				u1_t symbol				= 0;
				if_fail_fe(huffmanRead(input, literals, symbol));
				if(symbol < 256) {
					if_fail_fe(inflateByte(stream, output, produced++, adler, (u0_t)symbol));
					continue;
				}
				if(symbol == 256)
					rtrn 0;
				if_true_fe(symbol > 285);
				cnst u2_t lengthCode	= symbol - 257;
				u2_t extra				= 0;
				if(LENGTH_EXTRA[lengthCode])
					if_fail_fe(bitsRead(input, LENGTH_EXTRA[lengthCode], extra));
				cnst u2_t length		= LENGTH_BASE[lengthCode] + extra;

				u1_t distanceCode		= 0;
				if_fail_fe(huffmanRead(input, distances, distanceCode));
				if_true_fe(distanceCode >= ::gpk::size(DISTANCE_BASE));
				extra					= 0;
				if(DISTANCE_EXTRA[distanceCode])
					if_fail_fe(bitsRead(input, DISTANCE_EXTRA[distanceCode], extra));
				cnst u2_t distance		= DISTANCE_BASE[distanceCode] + extra;
				if_true_fe(0 == distance || distance > produced || distance > stream.Window.size());
				for(u2_t i = 0; i < length; ++i) {
					cnst u0_t value		= stream.Window[(u2_t)(produced - distance) & (stream.Window.size() - 1)];
					if_fail_fe(inflateByte(stream, output, produced++, adler, value));
				}
			}
		}

		sttc err_t		deflateWrite				(SZipStream & stream, au0_t & output) {
			SZlibHeader header			= {};
			header.Set(stream.Settings.WindowBits, stream.Settings.CompressionLevel);
			cnst u0_t headerBytes[]		= {header.Method.Encode(), header.Flags.Encode()};
			if_fail_fe(output.append(headerBytes));
			SBitWindow pending			= {};
			SDeflateBlockHeader block	= {};
			block.Final					= 1;
			block.Type					= DEFLATE_BLOCK_TYPE_FIXED;
			if_fail_fe(bitsWrite(pending, output, block.Encode(), block.BIT_COUNT));
			SDeflateHistory history		= deflateHistory(stream);
			SAdler32 adler				= {};
			for(u2_t offset = 0; offset < stream.Stream.size();) {
				cnst SDeflateMatch match	= matchFind(history, stream.Stream, offset, stream.Settings.SearchDepth);
				if(match.Match.Count >= 3) {
					cnst u2_t length		= (u2_t)match.Match.Count;
					if_fail_fe(matchWrite(pending, output, length, offset - (u2_t)match.Match.Offset));
					for(u2_t i = 0; i < length; ++i) {
						adlerAdd(adler, stream.Stream[offset + i]);
						matchInsert(history, stream.Stream, offset + i);
					}
					offset					+= length;
				}
				else {
					adlerAdd(adler, stream.Stream[offset]);
					if_fail_fe(fixedSymbolWrite(pending, output, stream.Stream[offset]));
					matchInsert(history, stream.Stream, offset++);
				}
			}
			if_fail_fe(fixedSymbolWrite(pending, output, 256));
			if_fail_fe(bitsFlush(pending, output));
			cnst u2_t checksum			= adlerValue(adler);
			cnst u0_t trailer[]			= {::gpk::byte_at(checksum, 3), ::gpk::byte_at(checksum, 2), ::gpk::byte_at(checksum, 1), ::gpk::byte_at(checksum, 0)};
			if_fail_fe(output.append(trailer));
			rtrn 0;
		}

		sttc err_t		inflateWrite				(SZipStream & stream, au0_t & output) {
			if_true_fe(stream.Stream.size() < 6);
			SZlibHeader header			= {};
			if_fail_fe(zlibHeaderRead(header, stream.Stream));
			cnst u2_t payloadSize		= stream.Stream.size() - 6;
			SBitReader input				= {{&stream.Stream[2], payloadSize}, {}};
			SAdler32 adler				= {};
			u3_t produced				= 0;
			bool finalBlock				= false;
			while(!finalBlock) {
				u2_t value				= 0;
				if_fail_fe(bitsRead(input, SDeflateBlockHeader::BIT_COUNT, value));
				SDeflateBlockHeader block	= {};
				block.Decode((u0_t)value);
				finalBlock				= 0 != block.Final;
				cnst DEFLATE_BLOCK_TYPE blockType = block.Type;
				if(DEFLATE_BLOCK_TYPE_STORED == blockType) {
					bitsAlign(input);
					u2_t length = 0, inverse = 0;
					if_fail_fe(bitsRead(input, 16, length));
					if_fail_fe(bitsRead(input, 16, inverse));
					if_true_fe((u1_t)length != (u1_t)~inverse);
					for(u2_t i = 0; i < length; ++i) {
						if_fail_fe(bitsRead(input, 8, value));
						if_fail_fe(inflateByte(stream, output, produced++, adler, (u0_t)value));
					}
				}
				else if(DEFLATE_BLOCK_TYPE_FIXED == blockType || DEFLATE_BLOCK_TYPE_DYNAMIC == blockType) {
					SHuffmanTable literals = {}, distances = {};
					if_fail_fe(DEFLATE_BLOCK_TYPE_FIXED == blockType ? fixedTables(literals, distances) : dynamicTables(input, literals, distances));
					if_fail_fe(inflateCompressed(stream, input, literals, distances, output, produced, adler));
				}
				else
					rtrn -1;
			}
			if_true_fe(input.Remaining.size());
			cnst u0_t * trailer			= &stream.Stream[stream.Stream.size() - 4];
			cnst u2_t expected			= ::gpk::byte_to<u2_t>(trailer[0], 3) + ::gpk::byte_to<u2_t>(trailer[1], 2) + ::gpk::byte_to<u2_t>(trailer[2], 1) + trailer[3];
			if_true_fe(expected != adlerValue(adler));
			rtrn 0;
	}
}

gpk::err_t			gpk::deflateInit			(SZipStream & output, vcu0_t input, SDeflateSettings settings) {
	output.Stream				= input;
	output.Settings				= settings;
	output.Status				= LIFECYCLE_STATE_FAILED;
	output.Window.clear();
	output.Previous.clear();
	output.Head.clear();
	if_fail_fe(::gpk::deflateSettingsValidate(settings));
	cnst u2_t windowSize			= ::gpk::bit_make<u2_t>(settings.WindowBits);
	cnst u2_t hashSize				= ::gpk::bit_make<u2_t>(settings.HashBits);
	if_fail_fe(output.Window.resize(windowSize, {}));
	if_fail_fe(output.Previous.resize(windowSize * szof(::gpk::SDeflateLink), {}));
	if_fail_fe(output.Head.resize(hashSize * szof(u2_t), {}));
	output.Status				= LIFECYCLE_STATE_READY;
	rtrn 0;
}

gpk::err_t			gpk::inflateInit			(SZipStream & output, vcu0_t input) {
	output.Stream				= input;
	output.Status				= LIFECYCLE_STATE_FAILED;
	SZlibHeader header			= {};
	if_fail_fe(::gpk::zlibHeaderRead(header, input));
	output.Window.clear();
	if_fail_fe(output.Window.resize(::gpk::bit_make<u2_t>(header.WindowBits()), {}));
	output.Previous.clear();
	output.Head.clear();
	output.Status				= LIFECYCLE_STATE_READY;
	rtrn 0;
}

gpk::err_t			gpk::arrayDeflate			(vcu0_t inflated, au0_t & deflated, SDeflateSettings settings, u2_c chunkSize)	{
#if defined(GPK_ESP32) || defined(GPK_ARDUINO)
	(void)settings;
#	ifdef GPK_ESP32
	deflated			= inflated;
#	else
	deflated			= inflated;
#	endif
#else
	(void)chunkSize;
	SZipStream stream				= {};
	if_fail_fe(deflateInit(stream, inflated, settings));
	stream.Status				= LIFECYCLE_STATE_BUSY;
	cnst err_t result				= ::gpk::deflateWrite(stream, deflated);
	stream.Status				= failed(result) ? LIFECYCLE_STATE_FAILED : LIFECYCLE_STATE_FINISHED;
	if_fail_fe(result);
#endif
	rtrn 0;
}
gpk::err_t			gpk::arrayDeflate			(vcu0_t inflated, au0_t & deflated, u2_c chunkSize)	{
	rtrn arrayDeflate(inflated, deflated, {}, chunkSize);
}
gpk::err_t			gpk::arrayInflate			(vcu0_t deflated, au0_t & inflated, u2_c chunkSize)	{
#if defined(GPK_ESP32) || defined(GPK_ARDUINO)
#	ifdef GPK_ESP32
	inflated			= deflated;
#	else
	inflated			= deflated;
#	endif
#else
	(void)chunkSize;
	SZipStream stream				= {};
	if_fail_fe(inflateInit(stream, deflated));
	stream.Status				= LIFECYCLE_STATE_BUSY;
	cnst err_t result				= ::gpk::inflateWrite(stream, inflated);
	stream.Status				= failed(result) ? LIFECYCLE_STATE_FAILED : LIFECYCLE_STATE_FINISHED;
	if_fail_fe(result);
#endif
	rtrn 0;
}


