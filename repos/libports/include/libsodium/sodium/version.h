/*
 * \brief  Dummy version header needed by libsodium
 * \author Stefan Kalkowski
 * \date   2016-02-11
 */

#ifdef __cplusplus
extern "C" {
#endif

void pbkdf2_sha256(const uint8_t *passwd, size_t passwdlen,
                   const uint8_t *salt,   size_t saltlen,
                   size_t iter,
                   uint8_t *buf, size_t buf_len);

#ifdef __cplusplus
}
#endif
