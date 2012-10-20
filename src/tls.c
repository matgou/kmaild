/******************************************************************************
*   Copyright (C) 2011 Mathieu Goulin <mathieu.goulin@gadz.org>               *
*                                                                             *
*   This program is free software; you can redistribute it and/or             *
*   modify it under the terms of the GNU Lesser General Public                *
*   License as published by the Free Software Foundation; either              *
*   version 2.1 of the License, or (at your option) any later version.        *
*                                                                             *
*   The GNU C Library is distributed in the hope that it will be useful,      *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
*   Lesser General Public License for more details.                           *
*                                                                             *
*   You should have received a copy of the GNU Lesser General Public          *
*   License along with the GNU C Library; if not, write to the Free           *
*   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA         *
*   02111-1307 USA.                                                           *
******************************************************************************/
/**
 * \file tls.c
 * \brief implements tls security
 * \author Goulin.M
 * \version 0.5
 * \date 23 avril 2011
 *
 */
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <smtpd-command.h>
#include <smtpd.h>

# define CLIENTCA "etc/clientca.pem"
# define CLIENTCRL "etc/clientcrl.pem"
# define SERVERCERT "etc/servercert.pem"

int verify_cb(int preverify_ok, X509_STORE_CTX *ctx) { return 1; }

int ssl_rfd = -1, ssl_wfd = -1; /* SSL_get_Xfd() are broken */
SSL *ssl = NULL;

RSA *tmp_rsa_cb(SSL *ssl, int export, int keylen)
{
  if (!export) keylen = 512;
  if (keylen == 512) {
    FILE *in = fopen("etc/rsa512.pem", "r");
    if (in) {
      RSA *rsa = PEM_read_RSAPrivateKey(in, NULL, NULL, NULL);
      fclose(in);
      if (rsa) return rsa;
    }
  }
  return RSA_generate_key(keylen, RSA_F4, NULL, NULL);
}

DH *tmp_dh_cb(SSL *ssl, int export, int keylen)
{
  if (!export) keylen = 1024;
  if (keylen == 512) {
    FILE *in = fopen("etc/dh512.pem", "r");
    if (in) {
      DH *dh = PEM_read_DHparams(in, NULL, NULL, NULL);
      fclose(in);
      if (dh) return dh;
    }
  }
  if (keylen == 1024) {
    FILE *in = fopen("etc/dh1024.pem", "r");
    if (in) {
      DH *dh = PEM_read_DHparams(in, NULL, NULL, NULL);
      fclose(in);
      if (dh) return dh;
    }
  }
  return DH_generate_parameters(keylen, DH_GENERATOR_2, NULL, NULL);
} 

/** 
 * \fn int starttls(char *c)
 * \brief handler for starttls
 */
int
starttls (char *c){
  SSL *myssl;
  SSL_CTX *ctx;
  X509_STORE *store;
  X509_LOOKUP *lookup;

  SSL_library_init();
  ctx = SSL_CTX_new(SSLv23_server_method());
  if (!ctx) {
    sendTxt("550 TLS error initializing ctx\r\n");
    return 550;
  }
  if (!SSL_CTX_use_certificate_chain_file(ctx, SERVERCERT))
    { SSL_CTX_free(ctx); sendTxt("550 missing certificate\r\n"); return 550; }
  SSL_CTX_load_verify_locations(ctx, CLIENTCA, NULL);

  store = SSL_CTX_get_cert_store(ctx);
  if ((lookup = X509_STORE_add_lookup(store, X509_LOOKUP_file())) &&
      (X509_load_crl_file(lookup, CLIENTCRL, X509_FILETYPE_PEM) == 1))
    X509_STORE_set_flags(store, X509_V_FLAG_CRL_CHECK |
                                X509_V_FLAG_CRL_CHECK_ALL);

  /* set the callback here; SSL_set_verify didn't work before 0.9.6c */
  SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, verify_cb);

  /* a new SSL object, with the rest added to it directly to avoid copying */
  myssl = SSL_new(ctx);
//  SSL_CTX_free(ctx);
  if (!myssl) { sendTxt("550 unable to initialize ssl\r\n"); return 550; }

  /* this will also check whether public and private keys match */
  if (!SSL_use_RSAPrivateKey_file(myssl, SERVERCERT, SSL_FILETYPE_PEM))
    { SSL_free(myssl); sendTxt("550 no valid RSA private key\r\n"); return 550; }
 

  SSL_set_tmp_rsa_callback(myssl, tmp_rsa_cb);
  SSL_set_tmp_dh_callback(myssl, tmp_dh_cb);

  SSL_set_rfd(myssl, ssl_rfd=fileno(stdin));
  SSL_set_wfd(myssl, ssl_wfd=fileno(stdout));
  sendTxt("220 ready for tls\r\n");

  int r=SSL_accept(myssl);
//  if(r<=0){
    int er = ERR_get_error();

    SSL_load_error_strings();
    const char *er_str= ERR_error_string(er, NULL);
    FILE *log=fopen("res.txt","w+");
    fputs(er_str,log);
    char *client_commands=malloc(sizeof(char)*2000);
    client_commands="\r\nHelloWord";
    client_commands[1]=getc(stdin);
    fputs(client_commands,log);
    fclose(log);
	sendTxt("550 tls negociation error %s \r\n", er_str);
        return 550;
/*  }
  SSL_set_mode(ssl, SSL_MODE_ENABLE_PARTIAL_WRITE);
  ssl = myssl;
  sendTxt("220 kapable.info\r\n");
  exit(0);
  return 220;*/
}

