/*
   Copyright 2020 Alessandro Ranellucci

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include "mbedtls/hkdf.h"
#include "mbedtls/md.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"

void setup()
{
  Serial.begin(115200);

  // generate the Tracing Key
  unsigned char TracingKey[32];
  CRNG(TracingKey, sizeof(TracingKey));

  // generate the Daily Tracing key
  unsigned char DailyTracingKey[16];
  {
    char info[8];
    sprintf(info, "CT-DTK%d", DayNumber());
    HKDF(TracingKey, sizeof(TracingKey), (unsigned char *)info,
         strlen(info), DailyTracingKey, sizeof(DailyTracingKey));
  }

  // generate a Rolling Proximity Identifier
  unsigned char RollingProximityIdentifier[16];
  {
    unsigned char hmacResult[32];
    char payload[9];
    sprintf(payload, "CT-RPI%d", TimeIntervalNumber());
    HMAC(DailyTracingKey, sizeof(DailyTracingKey),
         (unsigned char *)payload, sizeof(payload), hmacResult);
    memcpy(RollingProximityIdentifier, hmacResult, 16);
  }

  Serial.print("RPI: ");
  for (int i = 0; i < sizeof(RollingProximityIdentifier); i++)
  {
    char str[3];
    sprintf(str, "%02x", (int)RollingProximityIdentifier[i]);
    Serial.print(str);
  }
}

void loop() {}

void CRNG(unsigned char out[], size_t len)
{
  // https://tls.mbed.org/kb/how-to/generate-an-aes-key

  mbedtls_ctr_drbg_context ctr_drbg;
  mbedtls_entropy_context entropy;
  char *pers = "Arduino Contact Tracer";
  int ret;

  mbedtls_entropy_init(&entropy);
  mbedtls_ctr_drbg_init(&ctr_drbg);

  if ((ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                                   (unsigned char *)pers, strlen(pers))) != 0)
  {
    printf(" failed\n ! mbedtls_ctr_drbg_init returned -0x%04x\n", -ret);
  }

  if ((ret = mbedtls_ctr_drbg_random(&ctr_drbg, out, len)) != 0)
  {
    printf(" failed\n ! mbedtls_ctr_drbg_random returned -0x%04x\n", -ret);
  }
}

void HMAC(const unsigned char *key, size_t keyLength, const unsigned char *payload,
          size_t payloadLength, unsigned char hmacResult[])
{
  mbedtls_md_context_t ctx;
  mbedtls_md_type_t md_type = MBEDTLS_MD_SHA256;

  mbedtls_md_init(&ctx);
  mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 1);
  mbedtls_md_hmac_starts(&ctx, key, keyLength);
  mbedtls_md_hmac_update(&ctx, payload, payloadLength);
  mbedtls_md_hmac_finish(&ctx, hmacResult);
  mbedtls_md_free(&ctx);
}

int HKDF(const unsigned char *ikm, size_t ikm_len,
         const unsigned char *info, size_t info_len,
         unsigned char *okm, size_t okm_len)
{
  return mbedtls_hkdf(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), NULL,
                      0, ikm, ikm_len, info, info_len, okm, okm_len);
}

time_t epoch()
{
  return 1586692343;
}

uint32_t DayNumber()
{
  return epoch() / (60 * 60 * 24);
}

uint8_t TimeIntervalNumber()
{
  return (epoch() % (60 * 60 * 24) / (60 * 10));
}
