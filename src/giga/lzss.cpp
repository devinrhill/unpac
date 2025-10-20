#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#if !LZSS_NDEBUG
#include <format>
#include <iostream>
#endif
#include "lzss.hpp"

namespace giga {
namespace lzss {

int decompress(const std::uint8_t* code, // Code buffer
               std::size_t codeSize, // Code buffer size
               std::uint8_t* text, // Text buffer
               std::size_t textSize, // Text buffer size
               Config config // Reference field lengths
) {
    // Code buffer check
    if(!code) {
        return EBADCODE;
    }

    // Code buffer size check
    if(codeSize < 1) {
        return 0;
    }

    // Text buffer check
    if(!text) {
        return EBADTEXT;
    }
    // Text buffer size check
    if(textSize < 1) {
        return 0;
    }


    // Symbol reference length parameters sum must be 16 (2 bytes)
    if((config.EI + config.EJ) != 16) {
        return EBADCONFIG;
    }

    // Setting variables
    const std::size_t N = 1 << config.EI; // Size of the sliding window
    const std::uint32_t F = 1 << config.EJ; // Bitmask for wrapping reference length
    constexpr std::size_t P = 2; // Number of bytes for a symbol reference

    std::uint8_t ch; // Temporary symbol storage
    std::size_t refOffset, refLen; // Temporary reference offset and length
    std::uint8_t* window = nullptr; // Sliding window
    std::size_t windowOffset = (N - F) - P; // Offset in sliding window

#if !LZSS_NDEBUG
    std::cout << std::format("EI = {}, N = {}, EJ = {}, F = {}, windowOffset = {}", config.EI, N, config.EJ, F, windowOffset) << std::endl;
#endif

    // Sliding window allocation and setting
    window = reinterpret_cast<std::uint8_t*>(std::malloc(N));
    if(!window) {
        return EBADCONFIG;
    }
    std::memset(window, 0, N);

    const std::uint8_t* textStart = text;
    const std::uint8_t* codeEnd = code + codeSize;
    const std::uint8_t* textEnd = text + textSize;

    // Main decompression loop
    std::uint32_t flags = 0;
    for(;; flags >>= 1) {
        if(!(flags & 0x100)) { // Flag
            // Read flag byte from code buffer
            if(code >= codeEnd) break;
            flags = *code++;

            // Set reference flag
            flags |= 0xff00;
        }
        if(flags & 1) { // Literal symbol
            // Read literal symbol byte from code buffer
            if(code >= codeEnd) break;
            ch = *code++;

            // Put literal symbol byte into text buffer
            if(text >= textEnd) goto done;
            *text++ = ch;

            /* Put the last read literal symbol byte
               into the back of the sliding window */
            window[windowOffset] = ch;

            /* Increment the sliding window offset
               (limited to the sliding window size) */
            windowOffset = (windowOffset + 1) & (N - 1);
        } else { // Symbol reference
            // Read 2 symbol reference bytes from code buffer
            if(code >= codeEnd) break;
            refOffset = *code++;
            if(code >= codeEnd) break;
            refLen = *code++;

            // Bitmasks to obtain symbol reference offset and length
            refOffset |= ((refLen >> config.EJ) << 8);
            refLen = (refLen & (F - 1)) + P;

            // For each referenced symbol...
            for(std::size_t i = 0; i <= refLen; i++) {
                // Read the referenced symbol
                ch = window[(refOffset + i) & (N - 1)];

                // Put the referenced symbol into text buffer
                if(text >= textEnd) goto done;
                *text++ = ch;

                /* Put the last read literal symbol byte
                   into the back of the sliding window */
                window[windowOffset] = ch;

                /* Increment the sliding window offset
                   (limited to the sliding window size) */
                windowOffset = (windowOffset + 1) & (N - 1);
            }
        }
    }
done:
    // Free our sliding window buffer
    std::free(window);

    // Return the final size of text buffer
    return text - textStart;
}

class Compressor {
public:
    Compressor(std::uint8_t* code, std::size_t codeSize, std::uint8_t* text, std::size_t textSize, Config config);
    ~Compressor();

    int encode();

private:
    // General compression parameters
    int N;           // Size of sliding window
    int F;           // Upper limit for symbol match length
    const int P = 2; // Encode string into position and length if match_length is greater than this

    // Size counters
    std::size_t textsize = 0, // Text size counter
                codesize = 0; // Code size counter

    /* Ring buffer of size N with extra `F-1`
     * bytes to facilitate string comparison */
    std::uint8_t* text_buf = nullptr;

    int	match_position, match_length; // Of longest match

    // Binary search tree
    int  NIL;            // Index of root
    int *lson = nullptr, // Left children
        *rson = nullptr; // Right children
    int* dad = nullptr;  // Parents
    int treeSize = 256;
    
    std::uint8_t *code = nullptr,
                 *text = nullptr;
    std::size_t   codeSize = 0,
                  textSize = 0;

    std::uint8_t *textEnd = nullptr,
                 *codeEnd = nullptr;

    // Symbol input/output
    int getc();
    int putc(int ch);

    // Initialize trees
    void initTree();

    /* Inserts string of length F, text_buf[r..r+F-1], into one of the
     * trees (text_buf[r]'th tree) and returns the longest-match position
     * and length via the global variables match_position and match_length.
     * If match_length = F, then removes the old node in favor of the new
     * one, because the old one will be deleted sooner.
     * Note r plays double role, as tree node and position in buffer. */
    void insertNode(int r);

    // Deletes node p from tree
    void deleteNode(int p);
};

Compressor::Compressor(std::uint8_t* text, std::size_t textSize, std::uint8_t* code, std::size_t codeSize, Config config) {
    this->text = text;
    this->textSize = textSize;
    this->code = code;
    this->codeSize = codeSize;

    if(config.EI >= 16) {
        N = config.EI;
    } else {
        N = 1 << config.EI;
    }
    F = (1 << config.EJ) + P;

    NIL = N;
    text_buf = reinterpret_cast<std::uint8_t*>(std::malloc(N + F - 1));
    lson = reinterpret_cast<int*>(std::malloc(sizeof(int) * (N + 1)));
    rson = reinterpret_cast<int*>(std::malloc(sizeof(int) * (N + treeSize + 1)));
    dad = reinterpret_cast<int*>(std::malloc(sizeof(int) * (N + 1)));

    textEnd = this->text + textSize;
    codeEnd = this->code + codeSize;
}

Compressor::~Compressor() {
    std::free(text_buf);
    std::free(lson);
    std::free(rson);
    std::free(dad);
}

int Compressor::getc() {
    if(text >= textEnd) {
        return -1;
    }

    return *text++;
}

int Compressor::putc(int ch) {
    if(code >= codeEnd) {
        return -1;
    }

    *code++ = ch;
    return ch;
}

// Initialize trees
void Compressor::initTree() {
    int  i;

    /* For i = 0 to N - 1, rson[i] and lson[i] will be the right and
       left children of node i.  These nodes need not be initialized.
       Also, dad[i] is the parent of node i.  These are initialized to
       NIL (= N), which stands for 'not used.'
       For i = 0 to 255, rson[N + i + 1] is the root of the tree
       for strings that begin with character i.  These are initialized
       to NIL.  Note there are 256 trees. */

    for (i = N + 1; i <= N + treeSize; i++) {
        rson[i] = NIL;
    }

    for (i = 0; i < N; i++) {
        dad[i] = NIL;
    }
}

/* Inserts string of length F, text_buf[r..r+F-1], into one of the
 * trees (text_buf[r]'th tree) and returns the longest-match position
 * and length via the global variables match_position and match_length.
 * If match_length = F, then removes the old node in favor of the new
 * one, because the old one will be deleted sooner.
 * Note r plays double role, as tree node and position in buffer. */
void Compressor::insertNode(int r) {
    int i, p, cmp;
    std::uint8_t* key;

    cmp = 1;
    key = &text_buf[r];
    p = N + 1 + key[0];

    rson[r] = lson[r] = NIL;

    match_length = 0;
    for(;;) {
        if(cmp >= 0) {
            if(rson[p] != NIL) {
                p = rson[p];
            } else {
                rson[p] = r;
                dad[r] = p;

                return;
            }
        } else {
            if(lson[p] != NIL) {
                p = lson[p];
            } else {
                lson[p] = r;
                dad[r] = p;

                return;
            }
        }
        for(i = 1; i < F; i++) {
            if((cmp = key[i] - text_buf[p + i]) != 0) {
                break;
            }
        }

        if(i > match_length) {
            match_position = p;

            if((match_length = i) >= F) {
                break;
            }
        }
    }

    dad[r] = dad[p];
    lson[r] = lson[p];
    rson[r] = rson[p];
    dad[lson[p]] = r;
    dad[rson[p]] = r;

    if(rson[dad[p]] == p) {
        rson[dad[p]] = r;
    } else {
        lson[dad[p]] = r;
    }

    dad[p] = NIL; // Remove p
}

// Deletes node p from tree
void Compressor::deleteNode(int p) {
    int q;
    
    // Not in tree
    if(dad[p] == NIL) {
        return;
    }

    if(rson[p] == NIL) {
        q = lson[p];
    } else if(lson[p] == NIL) {
        q = rson[p];
    } else {
        q = lson[p];

        if(rson[q] != NIL) {
            do {
                q = rson[q];
            } while(rson[q] != NIL);

            rson[dad[q]] = lson[q];
            dad[lson[q]] = dad[q];
            lson[q] = lson[p];
            dad[lson[p]] = q;
        }

        rson[q] = rson[p];
        dad[rson[p]] = q;
    }

    dad[q] = dad[p];

    if(rson[dad[p]] == p) {
        rson[dad[p]] = q;
    } else {
        lson[dad[p]] = q;
    }

    dad[p] = NIL;
}

int Compressor::encode() {
    int i, c, len, r, s, last_match_length, code_buf_ptr;
    std::uint8_t mask;
    std::uint8_t* code_buf = reinterpret_cast<std::uint8_t*>(std::malloc(F - 1));
    if(!code_buf) {
        return -1;
    }

    initTree();

    /* code_buf[1..16] saves eight units of code, and
     * code_buf[0] works as eight flags, "1" representing that the unit
     * is an unencoded letter (1 byte), "0" a position-and-length pair
     * (2 bytes). Thus, eight units require at most 16 bytes of code. */
    code_buf[0] = 0;

    code_buf_ptr = mask = 1;
    s = 0;
    r = N - F;

    std::memset(text_buf, 0, r);

    for(len = 0; len < F && (c = getc()) != EOF; len++) {
        /* Read F bytes into the last
         * F bytes of the buffer */
        text_buf[r + len] = c;
    }

    // Text of size zero
    if((textsize = len) == 0) {
        return 0;
    }

    /* Insert the F strings,
     * each of which begins with one or more 'space' characters.  Note
     * the order in which these strings are inserted.  This way,
     * degenerate trees will be less likely to occur. */
    for(i = 1; i <= F; i++) {
        insertNode(r - i);
    }

    /* Finally, insert the whole string just read.  The
     * global variables match_length and match_position are set. */
    insertNode(r);

    do {
        /* match_length may be spuriously
         * long near the end of text. */
        if(match_length > len) {
            match_length = len;
        }

        if(match_length <= P) {
            match_length = 1; // Not long enough match.  Send one byte.
            code_buf[0] |= mask; // 'send one byte' flag
            code_buf[code_buf_ptr++] = text_buf[r]; // Send uncoded.
        } else {
            code_buf[code_buf_ptr++] = static_cast<unsigned char>(match_position);

            /* Send position and length pair.
             * Note match_length > THRESHOLD. */
            code_buf[code_buf_ptr++] = static_cast<unsigned char>
                ((((match_position >> 4) & 0xf0)
                | (match_length - (P + 1))));
        }

        if((mask <<= 1) == 0) { // Shift mask left one bit.
            // Send at-most 8 units of code together
            for(i = 0; i < code_buf_ptr; i++) {
                putc(code_buf[i]);
            }

            codesize += code_buf_ptr;

            code_buf[0] = 0;
            code_buf_ptr = mask = 1;
        }

        last_match_length = match_length;
        for(i = 0; i < last_match_length &&
            (c = getc()) != EOF; i++) {
            deleteNode(s); // Delete old strings
            text_buf[s] = c; // Read new bytes
            
            /* If the position is
             * near the end of buffer, extend the buffer to make
             * string comparison easier. */
            if(s < F - 1) {
                text_buf[s + N] = c;
            }

            /* Since this is a ring buffer,
             * increment position by modulo N */
            s = (s + 1) & (N - 1);
            r = (r + 1) & (N - 1);

            insertNode(r); // Register the string in text_buf[r..r+F-1]
        }

        // After the end of text
        while(i++ < last_match_length) {
            // No need to read, but buffer may not yet be empty.
            deleteNode(s);

            s = (s + 1) & (N - 1);
            r = (r + 1) & (N - 1);

            if(--len) {
                insertNode(r);
            }
        }
    } while(len > 0); // Until length of string to be processed is zero

    // Send remaining code.
    if(code_buf_ptr > 1) {
        for(i = 0; i < code_buf_ptr; i++) {
            putc(code_buf[i]);
        }

        codesize += code_buf_ptr;
    }

    std::free(code_buf);

    return codesize;
}

int compress(std::uint8_t* text, std::size_t textSize, std::uint8_t* code, std::size_t codeSize, Config config) {
    Compressor compressor(text, textSize, code, codeSize, config);
    
    int returnedCodeSize = compressor.encode();

#if !LZSS_NDEBUG
    std::cout << std::format("EI = {}, EJ = {}, returned code size = {}", config.EI, config.EJ, returnedCodeSize) << std::endl;
#endif

    return returnedCodeSize;
}

} // namespace lzss
} // namespace giga
