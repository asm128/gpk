#include "gpk_error.h"
#include <memory.h>

#ifndef	GPK_TRUETYPE_H_928734
#define GPK_TRUETYPE_H_928734

inline uint16_t BE2LE(uint16_t value) {
	byte_t raw[2];
	memcpy(raw, &value, 2);
	return static_cast<uint16_t>( (raw[0] << 8) | (raw[1]) );
}

inline int16_t BE2LE(int16_t value) {
	byte_t raw[2];
	memcpy(raw, &value, 2);
	return static_cast<int16_t>( (raw[0] << 8) | (raw[1]) );
}

inline unsigned int BE2LE(uint32_t value) {
	byte_t raw[4];
	memcpy(raw, &value, 4);
	return static_cast<uint32_t>( (raw[0] << 24) | (raw[1] << 16) | (raw[2] << 8) | (raw[3]) );
}

#define	LE2BE(x)	BE2LE(x)

struct STTOffsetTable {
	uint32_t				Version;
	uint16_t				NumTables;		// number of tables
	uint16_t				SearchRange;	// (maximum power of 2 <= numTables)*16
	uint16_t				EntrySelector;	// log2(maximum power of 2 <= numTables)
	uint16_t				RangeShift;		// numTables*16-searchRange
};

struct STTTableDirectory {
	byte_t					Tag[4];		// 4-byte identifier
	uint32_t				CheckSum;	// checksum for this table
	uint32_t				Offset;		// offset from beginning of sfnt
	uint32_t				Length;		// length of this table in byte (actual length not padded length)
};

// http://developer.apple.coM/fonts/TTRefMan/RM06/Chap6.html
struct STTCmapIndex {
	uint16_t				Version;			// Version number (Set to zero)
	uint16_t				NumberSubtables;	// Number of encoding subtables
};

struct STTCmapEncodingSubtable {
	uint16_t				PlatformID;			// Platform identifier
	uint16_t				PlatformSpecificID;	// Platform-specific encoding identifier
	uint32_t				Offset;				// Offset of the mapping table
};

struct STTCmapFormat0Table {
	uint16_t				Format;					// Set to 0
	uint16_t				Length;					// Length in bytes of the subtable (set to 262 for format 0)
	uint16_t				Language;				// Language code for this encoding subtable, or zero if language-independent
	byte_t					GlyphIndexArray[256];	// An array that maps character codes to glyph index values
};

struct STTCmapFormat6Table {
	uint16_t				Format;						// Format number is set to 6
	uint16_t				Length;						// Length in bytes
	uint16_t				Language;					// Language code for this encoding subtable, or zero if language-independent
	uint16_t				FirstCode;					// First character code of subrange
	uint16_t				EntryCount;					// Number of character codes in subrange
	// Array o glyph index Values for character codes in the range
};

// Glyph data
struct STTGlyfTable {
	int16_t					NumberOfContours;			// If the number of contours is positive or zero, it is a single glyph; If the number of contours is -1, the glyph is compound 
	int16_t					XMin;						// Minimum x for coordinate data 
	int16_t					YMin;						// Minimum y for coordinate data 
	int16_t					XMax;						// Maximum x for coordinate data 
	int16_t					YMax;						// Maximum y for coordinate data
};

// Header
struct STTHeadTable {
	uint32_t				Version;					// 0x00010000 if (version 1.0)
	uint32_t				FontRevision;				// set by font manufacturer
	uint32_t				CheckSumAdjust;				// To compute: set it to 0, calculate the checksum for the 'head' table and put it in the table directory, sum the entire font as uint32, then store B1B0AFBA - sum. The checksum for the 'head' table will not be wrong. That is OK.
	uint32_t				MagicNumber;				// set to 0x5F0F3CF5
	uint16_t				Flags;			
	uint16_t				UnitsPerEM;					// range from 64 to 16384
	int32_t					Created[2];					// international date
	int32_t					Modified[2];				// international date
	int16_t					XMin;						// for all glyph bounding boxes
	int16_t   				YMin;						// for all glyph bounding boxes
	int16_t   				XMax;						// for all glyph bounding boxes
	int16_t   				YMax;						// for all glyph bounding boxes
	uint16_t				MacStyle;
	uint16_t				LowestRecPPEM;
	int16_t					FontDirection;
	int16_t					IndexToLocFormat;	// 0 for short offsets, 1 for long
	int16_t					GlyphDataFormat;	// 0 for current format
};

// Horizontal eader
struct STTHHeaTable {
	uint32_t				Version;				// 0x00010000 (1.0)
	int16_t					Ascent;					// Distance from baseline of highest ascender
	int16_t					Descent;				// Distance from baseline of lowest descender
	int16_t					LineGap;				// typographic line gap
	uint16_t				AdvanceWidthMax;		// must be consistent with horizontal metrics
	int16_t					MinLeftSideBearing;		// must be consistent with horizontal metrics
	int16_t					MinRightSideBearing;	// must be consistent with horizontal metrics
	int16_t					XMaxExtent;				// max(lsb + (xMax-xMin))
	int16_t					CaretSlopeRise;			// used to calculate the slope of the caret (rise/run) set to 1 for vertical caret
	int16_t					CaretSlopeRun;			// 0 for vertical
	int16_t					CaretOffset;			// set value to 0 for non-slanted fonts
	int16_t					Reserved[4];			// set value to 0
	int16_t					MetricDataFormat;		// 0 for current format
	uint16_t				NumberOfHMetrics;		// number of advance widths in metrics table
};

// Horizontal etrics
struct STTHmtxEntry {
	uint16_t				AdvanceWidth;
	int16_t					LeftSideBearing;
};

// "loca"

// Maximum profile
struct STTMaxpTable {
	uint32_t				Version;				// 0x00010000 (1.0)
	uint16_t				NumGlyphs;				// the number of glyphs in the font
	uint16_t				MaxPoints;				// points in non-compound glyph
	uint16_t				MaxContours;			// contours in non-compound glyph
	uint16_t				MaxComponentPoints;		// points in compound glyph
	uint16_t				MaxComponentContours;	// contours in compound glyph
	uint16_t				MaxZones;				// set to 2
	uint16_t				MaxTwilightPoints;		// points used in Twilight Zone (Z0)
	uint16_t				MaxStorage;				// number of Storage Area locations
	uint16_t				MaxFunctionDefs;		// number of FDEFs
	uint16_t				MaxInstructionDefs;		// number of IDEFs
	uint16_t				MaxStackElements;		// maximum stack depth
	uint16_t				MaxSizeOfInstructions;	// byte count for glyph instructions
	uint16_t				MaxComponentElements;	// number of glyphs referenced at top level
	uint16_t				MaxComponentDepth;		// levels of recursion, set to 0 if font has only simple glyphs
};

struct STTNameRecord {
	uint16_t				PlatformID;			// Platform identifier code.
	uint16_t				PlatformSpecificID;	// Platform-specific encoding identifier.
	uint16_t				LanguageID;			// Language identifier.
	uint16_t				NameID;				// Name identifiers.
	uint16_t				Length;				// Name string length in bytes.
	uint16_t				Offset;				// Name string offset in bytes from stringOffset.
};

// Name
struct STTNameTable {
	uint16_t				format;			// Format selector. Set to 0.
	uint16_t				count;			// The number of nameRecords in this name table.
	uint16_t				stringOffset;	// Offset in bytes to the beginning of the name character strings.
	// The nam records array.
};

// PostScript
struct STTPostTable {
	uint32_t				format;				// Format of this table
	uint32_t				italicAngle;		// Italic angle in degrees
	int16_t					underlinePosition;	// Underline position
	int16_t					underlineThickness;	// Underline thickness
	uint32_t				isFixedPitch;		// Font is monospaced; set to 1 if the font is monospaced and 0 otherwise (N.B., to maintain compatibility with older versions of the TrueType spec, accept any non-zero value as meaning that the font is monospaced)
	uint32_t				minMemType42;		// Minimum memory usage when a TrueType font is downloaded as a Type 42 font
	uint32_t				maxMemType42;		// Maximum memory usage when a TrueType font is downloaded as a Type 42 font
	uint32_t				minMemType1;		// Minimum memory usage when a TrueType font is downloaded as a Type 1 font
	uint32_t				maxMemType1;		// Maximum memory usage when a TrueType font is downloaded as a Type 1 font
};

//Atencion: esta tabla solo es requerida en fonts que son para windows.
struct STTOs2Table {
	uint16_t				version;				// table version number (set to 0)
	int16_t					xAvgCharWidth;			// average weighted advance width of lower case letters and space
	uint16_t				usWeightClass;			// visual weight (degree of blackness or thickness) of stroke in glyphs
	uint16_t				usWidthClass;			// relative change from the normal aspect ratio (width to height ratio) as specified by a font designer for the glyphs in the font
	int16_t					fsType;					// characteristics and properties of this font (set undefined bits to zero)
	int16_t					ySubscriptXSize;		// recommended horizontal size in pixels for subscripts
	int16_t					ySubscriptYSize;		// recommended vertical size in pixels for subscripts
	int16_t					ySubscriptXOffset;		// recommended horizontal offset for subscripts
	int16_t					ySubscriptYOffset;		// recommended vertical offset form the baseline for subscripts
	int16_t					ySuperscriptXSize;		// recommended horizontal size in pixels for superscripts
	int16_t					ySuperscriptYSize;		// recommended vertical size in pixels for superscripts
	int16_t					ySuperscriptXOffset;	// recommended horizontal offset for superscripts
	int16_t					ySuperscriptYOffset;	// recommended vertical offset from the baseline for superscripts
	int16_t					yStrikeoutSize;			// width of the strikeout stroke
	int16_t					yStrikeoutPosition;		// position of the strikeout stroke relative to the baseline
	int16_t					sFamilyClass;			// classification of font-family design.
	byte_t					panose[10];				// 10 byte series of number used to describe the visual characteristics of a given typeface
	uint32_t				ulCharRange[4];			// Field is split into two bit fields of 96 and 36 bits each. The low 96 bits are used to specify the Unicode blocks encompassed by the font file. The high 32 bits are used to specify the character or script sets covered by the font file. Bit assignments are pending. Set to 0
	byte_t					achVendID[4];			// four character identifier for the font vendor
	uint16_t				fsSelection;			// 2-byte bit field containing information concerning the nature of the font patterns
	uint16_t				fsFirstCharIndex;		// The minimum Unicode index in this font.
	uint16_t				fsLastCharIndex;		// The maximum Unicode index in this font.
};

struct STTPcltTable {
	int16_t					version;
	uint32_t				fontNumber;
	uint16_t				pitch;
	uint16_t				xHeight;
	uint16_t				style;
	uint16_t				typeFamily;
	uint16_t				capHeight;
	uint16_t				symbolSet;
	byte_t					typeFace[16];
	byte_t					characterComplement[8];
	byte_t					fileName[6];
	byte_t					strokeWeight;
	byte_t					widthType;
	byte_t					serifStyle;
	byte_t					reserved;
};

#endif // GPK_TRUETYPE_H_928734
