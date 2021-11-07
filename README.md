# VM setting for IDF platform (Ubuntu 18.04.6)

## I. Steps for machine setup:

1. **Setup open SSH**

   https://linuxize.com/post/how-to-enable-ssh-on-ubuntu-18-04/

2. **Get IP address**

   ifconfig -> sudo apt install net-tools

3. **Connect SSH by VScode**

4. **Git install: sudo apt install git**

5. **Copy cert for GitHub**



## II. Steps for IDF setup

1. **Clone project at: https://github.com/ThuanLeUte/pet-device-fw**

2. **Git submodule setup:**

   git submodule init

   git submodule update --recursive

3. **Run setup.sh**

4. **Build, flash and log monitor:**

   Go to app directory  execute following command:

   make -j4 flash monitor



## **Note:** Guide how to fix the following issue

CONFIG_MBEDTLS_CERTIFICATE_BUNDLE=n

https://github.com/espressif/esp-idf/issues/7631



# ST25R3911B reference source code

https://www.st.com/content/st_com/en/products/embedded-software/st25-nfc-rfid-software/stsw-st25r002.html

https://www.st.com/content/st_com/en/products/embedded-software/st25-nfc-rfid-software/stsw-stm8-nfc5.html