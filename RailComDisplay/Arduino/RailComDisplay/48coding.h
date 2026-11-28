// LUT for 4/8 coding
// https://github.com/bakerstu/openmrn/blob/master/src/dcc/RailCom.cxx

static constexpr uint8_t INV = 0xff;
static constexpr uint8_t ACK = 0xfe;
static constexpr uint8_t NACK = 0xfd;
static constexpr uint8_t BUSY = 0xfc;
static constexpr uint8_t RESVD1 = 0xfb;
static constexpr uint8_t RESVD2 = 0xfa;

const uint8_t railcom_decode[256] =
    // 0|8     1|9     2|a     3|b     4|c     5|d     6|e     7|f  
{      INV,    INV,    INV,    INV,    INV,    INV,    INV,    INV, // 0
       INV,    INV,    INV,    INV,    INV,    INV,    INV,    ACK, // 0
       INV,    INV,    INV,    INV,    INV,    INV,    INV,   0x33, // 1
       INV,    INV,    INV,   0x34,    INV,   0x35,   0x36,    INV, // 1
       INV,    INV,    INV,    INV,    INV,    INV,    INV,   0x3A, // 2
       INV,    INV,    INV,   0x3B,    INV,   0x3C,   0x37,    INV, // 2
       INV,    INV,    INV,   0x3F,    INV,   0x3D,   0x38,    INV, // 3
       INV,   0x3E,   0x39,    INV,   NACK,    INV,    INV,    INV, // 3
       INV,    INV,    INV,    INV,    INV,    INV,    INV,   0x24, // 4
       INV,    INV,    INV,   0x23,    INV,   0x22,   0x21,    INV, // 4
       INV,    INV,    INV,   0x1F,    INV,   0x1E,   0x20,    INV, // 5
       INV,   0x1D,   0x1C,    INV,   0x1B,    INV,    INV,    INV, // 5
       INV,    INV,    INV,   0x19,    INV,   0x18,   0x1A,    INV, // 6
       INV,   0x17,   0x16,    INV,   0x15,    INV,    INV,    INV, // 6
       INV,   0x25,   0x14,    INV,   0x13,    INV,    INV,    INV, // 7
      0x32,    INV,    INV,    INV,    INV,    INV,    INV,    INV, // 7
       INV,    INV,    INV,    INV,    INV,    INV,    INV, RESVD2, // 8
       INV,    INV,    INV,   0x0E,    INV,   0x0D,   0x0C,    INV, // 8
       INV,    INV,    INV,   0x0A,    INV,   0x09,   0x0B,    INV, // 9
       INV,   0x08,   0x07,    INV,   0x06,    INV,    INV,    INV, // 9
       INV,    INV,    INV,   0x04,    INV,   0x03,   0x05,    INV, // a
       INV,   0x02,   0x01,    INV,   0x00,    INV,    INV,    INV, // a
       INV,   0x0F,   0x10,    INV,   0x11,    INV,    INV,    INV, // b
      0x12,    INV,    INV,    INV,    INV,    INV,    INV,    INV, // b
       INV,    INV,    INV, RESVD1,    INV,   0x2B,   0x30,    INV, // c
       INV,   0x2A,   0x2F,    INV,   0x31,    INV,    INV,    INV, // c
       INV,   0x29,   0x2E,    INV,   0x2D,    INV,    INV,    INV, // d
      0x2C,    INV,    INV,    INV,    INV,    INV,    INV,    INV, // d
       INV,   BUSY,   0x28,    INV,   0x27,    INV,    INV,    INV, // e
      0x26,    INV,    INV,    INV,    INV,    INV,    INV,    INV, // e
       ACK,    INV,    INV,    INV,    INV,    INV,    INV,    INV, // f
       INV,    INV,    INV,    INV,    INV,    INV,    INV,    INV, // f       
};