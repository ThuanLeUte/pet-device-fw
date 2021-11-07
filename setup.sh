#!/bin/bash

CURR_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
CURR_SHELL="$(basename -- $SHELL)"
SHELL_CFG="$HOME/.${CURR_SHELL:-bash}rc"
XTENSA_PATH="sdk/xtensa-esp32-elf/bin"
DEFAULT_IDF_PATH="$CURR_DIR/sdk/esp-idf"

# Install required packages
sudo apt-get install python3-pip
sudo apt-get install -y gcc git wget make libncurses-dev flex bison gperf python python-pip python-setuptools python-serial python-cryptography python-future python-pyparsing libffi-dev libssl-dev

# Make sure $USER have access to USB port
sudo usermod -a -G dialout $USER

# Update repo submodules
git submodule update --init --recursive

echo "INFO: CURR_SHELL = $CURR_SHELL"
echo "INFO: SHELL_CFG = $SHELL_CFG"

# Check if xtensa path is exported or not
if [[ "$PATH" == *"$XTENSA_PATH"* ]]; then
  echo "INFO: Xtensa Compiler FOUND"
else
  echo "INFO: Xtensa Compiler NOT FOUND. Exporting..."
  echo -e "export PATH=\"\$PATH:$CURR_DIR/$XTENSA_PATH\"" >> "$SHELL_CFG"
fi

# Check if IDF_PATH existed or not
if [[ -z "${IDF_PATH}" ]];
then
  echo "INFO: IDF_PATH NOT FOUND. Exporting..."
  echo -e "export IDF_PATH=$DEFAULT_IDF_PATH" >> "$SHELL_CFG"
  
  # Install Python requirements 
  python2 -m pip install --user -r $DEFAULT_IDF_PATH/requirements.txt
else
  echo "INFO: IDF_PATH FOUND!"
fi

# Check if ADF_PATH existed or not
if [[ -z "${ADF_PATH}" ]];
then
  echo "INFO: ADF_PATH NOT FOUND. Exporting..."
  bash $DEFAULT_IDF_PATH/install.sh
  echo -e "source $DEFAULT_IDF_PATH/export.sh" >> "$SHELL_CFG"
else
  echo "INFO: ADF_PATH FOUND!"
fi

# ✔: \xe2\x9c\x93
# ✗: \xe2\x9c\x97
echo -e "\xe2\x9c\x93 Setup OK. Please log out!"
