#include "../../crypto_pwhash/scryptsalsa208sha256/pbkdf2-sha256.h"

void pbkdf2_sha256(const uint8_t *passwd, size_t passwdlen,
                   const uint8_t *salt,   size_t saltlen,
                   size_t iter,
                   uint8_t *buf, size_t buf_len)
{
	PBKDF2_SHA256(passwd, passwdlen, salt, saltlen, iter, buf, buf_len);
}
