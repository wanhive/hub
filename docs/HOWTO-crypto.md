# Self signed certificate with own root CA

REF: [Self signed certificate with custom ca](https://gist.github.com/fntlnz/cf14feb5a46b2eda428e000157447309)

## Root CA

If you want a non password protected key just remove the **-des3** option

```
openssl genrsa -des3 -out rootCA.key 4096
openssl req -x509 -new -nodes -key rootCA.key -sha256 -days 1024 -out rootCA.crt
```

## Server Key and CSR

```
openssl genrsa -out wh.key 2048
openssl req -new -key wh.key -out wh.csr
```

## Verify the CSR

```
openssl req -in wh.csr -noout -text
```

## Sign with root CA
* The first time you use your CA to sign a certificate you can use the **-CAcreateserial** option. 
* This option will create a file (ca.srl) containing a serial number. 
* You are probably going to create more certificate, and the next time you will have to do that use 
the **-CAserial** option (and no more -CAcreateserial) followed with the name of the file containing 
your serial number. This file will be incremented each time you sign a new certificate. 
* This serial number will be readable using a browser (once the certificate is imported to a pkcs12 format). 
And we can have an idea of the number of certificate created by a CA.

```
openssl x509 -req -in wh.csr -CA rootCA.crt -CAkey rootCA.key -CAcreateserial -out wh.crt -days 365 -sha256

openssl x509 -req -in wh.csr -CA rootCA.crt -CAkey rootCA.key -CAserial rootCA.srl -out wh.crt -days 365 -sha256
```

##Verify the certificate

```
openssl x509 -in wh.crt -text -noout
```

**TIP**: Use c_rehash to hash the pem files


# JAVA Client: generating PKCS certificate

```
keytool -import -v -trustcacerts -alias WanhiveTrust -keypass <password1> -file rootCA.crt -keystore rootCA.jks -storepass <password2>
```

## Verify the certificate

```
keytool -list -keystore rootCA.jks -storepass <password2> -alias WanhiveTrust -v
```

## System properties

* -Djavax.net.ssl.keyStoreType=pkcs12
* -Djavax.net.ssl.trustStoreType=jks
* -Djavax.net.ssl.keyStore=clientcert.p12
* -Djavax.net.ssl.trustStore=trust.keystore
* -Djavax.net.debug=ssl
* -Djavax.net.ssl.keyStorePassword=xxxx
* -Djavax.net.ssl.trustStorePassword=yyyy

