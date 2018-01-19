/*
 * Part of tivars_lib_cpp
 * (C) 2015-2016 Adrien 'Adriweb' Bertrand
 * https://github.com/adriweb/tivars_lib_cpp
 * License: MIT
 */

#include "../autoloader.h"

// TODO : check if the models have different exponent offsets

using namespace std;

namespace
{
    static bool parseSign(string::const_iterator &i, const string::const_iterator &e) {
      bool sign = false;
      if (i != e && (*i == '+' || *i == '-')) {
        sign = *i++ == '-';
      }
      if (i == e) {
        std::cerr << "parseSign: Unexpected end of string." << endl;
        return false;
      }
      return sign;
    }
}

namespace tivars
{
    
    data_t TH_0x00::makeDataFromString(const string& str, const options_t& options)
    {
        (void)options;

        data_t data(dataByteCount);
        bool beforePoint = true, noDigits = true, zero = true;
        int exponent = 0x7F;
        unsigned index = 4;
        string::const_iterator i = str.begin();
        const string::const_iterator e = str.end();
        if (parseSign(i, e)) {
          data[0] = 1 << 7;
        }
        do {
          char c = *i++;
          if (c == '.') {
            if (!beforePoint) {
              std::cerr << "TH_0x00::makeDataFromString: Extra decimal points." << endl;
              return data_t{};
            }
            beforePoint = false;
          } else if (c == '0') {
            noDigits = false;
            if (zero) {
              exponent -= !beforePoint;
            } else {
              exponent += beforePoint;
              index += index <= dataByteCount << 1;
            }
          } else if (c >= '1' && c <= '9') {
            noDigits = zero = false;
            exponent += beforePoint;
            if (index < dataByteCount << 1) {
              data[index >> 1] |= (c - '0') << ((~index & 1) << 2);
              index++;
            } else if (index == dataByteCount << 1) {
              if (c >= '5') {
                while (true) {
                  if (--index < 4) {
                    data[2] = 0x10;
                    exponent++;
                    break;
                  }
                  if ((data[index >> 1] >> ((~index & 1) << 2) & 0xF) < 9) {
                    data[index >> 1] += 1 << ((~index & 1) << 2);
                    break;
                  }
                  data[index >> 1] &= 0xF << ((index & 1) << 2);
                }
              }
              index = (dataByteCount << 1) + 1;
            }
          } else if (c == 'e') {
            bool sign = parseSign(i, e);
            int offset = 0;
            do {
              char c = *i++;
              if (c >= '0' && c <= '9') {
                offset *= 10;
                offset += c - '0';
              } else {
                std::cerr << "TH_0x00::makeDataFromString: Unexpected character." << endl;
                return data_t{};
              }
            } while (i != e);
            exponent = sign ? exponent - offset : exponent + offset;
          } else {
              std::cerr << "TH_0x00::makeDataFromString: Unexpected character." << endl;
              return data_t{};
          }
        } while (i != e);
        if (noDigits) {
          std::cerr << "TH_0x00::makeDataFromString: No digits found." << endl;
          return data_t{};
        }
        if (exponent < 0x80 - 99 || exponent > 0x80 + 99) {
          std::cerr << "TH_0x00::makeDataFromString: Exponent out of range." << endl;
          return data_t{};
        }
        data[1] = exponent;
        return data;
    }

    string TH_0x00::makeStringFromData(const data_t& data, const options_t& options)
    {
        (void)options;

        if (data.size() != TH_0x00::dataByteCount)
        {
            std::cerr << "TH_0x00::makeDataFromString: Invalid data array. Needs to contain " + to_string(TH_0x00::dataByteCount) + " bytes" << endl;
            return "";
        }

        uint flags      = data[0];
        bool isNegative = (flags >> 7 == 1);
//      bool isSeqInit  = (flags  & 1 == 1); // if true, "used for initial sequence values"
        int exponent    = data[1] - 0x80;
        unsigned numLeadingZeros = 0, pointPos = 4;
        bool scientific = false;
        if (exponent < -3 || exponent > 9) {
          scientific = true;
        } else {
          if (exponent < 0) {
            numLeadingZeros -= exponent;
          }
          pointPos += exponent;
        }
        char result[21], *i = result;
        if (isNegative) {
          *i++ = '-';
        }
        for (unsigned index = 4 - numLeadingZeros; index < dataByteCount << 1; index++) {
          *i++ = '0' + (index < 4 ? 0 : data[index >> 1] >> ((~index & 1) << 2) & 0xF);
          if (index == pointPos) {
            *i++ = '.';
          }
        }
        while (--i != result && *i == '0') {
        }
        if (*i == '.') {
          i--;
        }
        if (scientific) {
          *++i = 'e';
          if (exponent < 0) {
            exponent = -exponent;
            *++i = '-';
          }
          if (exponent > 9) {
            *++i = '0' + exponent / 10;
            exponent = exponent % 10;
          }
          *++i = '0' + exponent;
        }
        *++i = '\0';
        return string(result);
    }
}
