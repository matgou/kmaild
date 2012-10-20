#include <stdio.h>
#include <stdlib.h>
#include <openssl/ssl.h>
int smtps=0;
int tls_init()
{
  SSL *myssl;
  SSL_CTX *ctx;
  SSL_library_init();
  ctx = SSL_CTX_new(SSLv23_client_method());
# define CLIENTCERT "control/clientcert.pem"
  if (SSL_CTX_use_certificate_chain_file(ctx, CLIENTCERT))
    SSL_CTX_use_RSAPrivateKey_file(ctx, CLIENTCERT, SSL_FILETYPE_PEM);
# undef CLIENTCERT
  myssl = SSL_new(ctx);
  SSL_CTX_free(ctx);
  printf("STARTTLS\r\n");
  fflush(stdout);
  SSL_set_wfd(myssl, fileno(stdout));
  SSL_set_rfd(myssl, fileno(stdin));
  sleep(1);
  SSL_connect(myssl);
  SSL_set_mode(myssl, SSL_MODE_ENABLE_PARTIAL_WRITE);

  return 1;
}
int
main (int argc, char *argv[])
{
  if (argc < 2)
    {
      printf ("usage smtpt count");
      exit (1);
    }
  int i;
  sleep(1);
  printf ("EHLO monet.kapable.info\r\n");
  fflush(stdout);
  tls_init();
/*  for (i = 0; i < atoi (argv[1]); i++)
    {
      printf ("MAIL FROM:<kapable@monet.kapable.info>\r\n");
      fflush(stdout);
      printf ("RCPT TO:<kapable@monet.kapable.info>\r\n");
      fflush(stdout);
      printf ("DATA\r\n");
      printf ("Date: Wed, 13 Apr 2011 11:10:45 +0200\r\n");
      printf ("To: kapable@gadz.org\r\n");
      printf ("From: kapable@monet.kapable.info\r\n");
      printf ("Subject: test Wed, 13 Apr 2011 11:10:45 +0200\r\n");
      printf ("X-Mailer: swaks v20100211.0 jetmore.org/john/code/swaks/\r\n");
      printf ("\r\n");
      printf ("This is a test mailing\r\n");
      printf ("\r\n.\r\n");
      fflush(stdout);
    }
  printf ("QUIT\r\n");
  fflush(stdout);
*/
  for(;;) {}
  return 1;
}
