# ChangeLog
## 0.0.1 2025-02-25
CHANGELOG.md created. Directory unit_tests in project root made to house all unit 
tests in central location.
Changed linux.CMakeLists.txt -> CMakeLists.txt.template
    - Please use the template in the future
    - Added network configuration and LSP stuff in the template

IMPORTANT:
Add SSID and WIFI_PASS to PATH:
```
echo 'export SSID="my_wifi_ssid"' >> ~/.bashrc
echo 'export WIFI_PASS="my_wifi_password"' >> ~/.bashrc
```
CMake will inject these into source code.

