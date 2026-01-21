# Self signed certificate with own root CA

## 1. Root CA Private Key:

**Create a ED25519 private key, protecting it with AES-256 encryption.**

```
openssl genpkey -algorithm ED25519 -out ca.key -aes256
```

**Or without passphrase for simplicity (less secure for production CA):**

```
openssl genpkey -algorithm ED25519 -out ca.key
```

-----------------------------------------------------

## 2. Root CA's Self-Signed Certificate:

**Create the root certificate, valid for ten (10) years.**

```
openssl req -new -x509 -key ca.key -out ca.crt -days 3650 -sha256
```

-----------------------------------------------------

## 3. Server's Private Key:

**Create the server's own private key for its certificate.**

```
openssl genpkey -algorithm ED25519 -out server.key
```

-----------------------------------------------------

## 4. Server's Certificate Signing Request (CSR):

```
openssl req -new -key server.key -out server.csr
```

**Verify the CSR:**

```
openssl req -in server.csr -noout -text
```

*This defines your server's identity.*

-----------------------------------------------------

## 5. Sign the Server's CSR with root CA:

* The first time you use your CA to sign a certificate you can use the **-CAcreateserial** option. 
This option will create a file (ca.srl) containing a serial number. 
* You are probably going to create more certificate, and the next time you will have to do that 
use the **-CAserial** option (and no more **-CAcreateserial**) followed with the name of the file 
containing your serial number. This file will be incremented each time you sign a new certificate. 
* This serial number will be readable using a browser (once the certificate is imported to a pkcs12 format). 
And we can have an idea of the number of certificate created by a CA.

```
openssl x509 -req -in server.csr -CA ca.crt -CAkey ca.key -CAcreateserial -out server.crt -days 365 -sha256
```

**Verify the certificate:**

```
openssl x509 -in server.crt -text -noout
```

**TIP**: Use `c_rehash` to hash the pem files.

*This uses ca.crt to sign server.csr, creating server.crt (your signed certificate).*

*Now you have ca.crt (the trusted root), server.key (server's private key), and server.crt (server's signed certificate).*


# JAVA Client: generating PKCS certificate

```
keytool -import -v -trustcacerts -alias WanhiveTrust -keypass password1 -file ca.crt -keystore ca.jks -storepass password2
```

### Verify the certificate

```
keytool -list -keystore ca.jks -storepass password2 -alias WanhiveTrust -v
```

### System properties

* -Djavax.net.ssl.keyStoreType=pkcs12
* -Djavax.net.ssl.trustStoreType=jks
* -Djavax.net.ssl.keyStore=clientcert.p12
* -Djavax.net.ssl.trustStore=trust.keystore
* -Djavax.net.debug=ssl
* -Djavax.net.ssl.keyStorePassword=password1
* -Djavax.net.ssl.trustStorePassword=password2

# References

* [ Self Signed Certificate with Custom Root CA](https://gist.github.com/fntlnz/cf14feb5a46b2eda428e000157447309)
