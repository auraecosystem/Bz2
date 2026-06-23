# Decompress the lz file on the fly and pipe it directly into Bash to run it
lzip -cd script.lz | bash
lzip -d script.lz       # Extracts to script (or script.sh)
chmod +x script         # Grants execution permissions
./script                # Runs the script
