Takologic v0.1
A STM32F103C8 Based logic analyzer

Kuro
kuro@lzzbr.com

This project is still under development.
Everything is release under GPLv3 unless other licenses are involved, such as STM Peripheral Libs.

Communication are through USART2 (pins A2 and A3) at 115200baud for now, USB is intended for newer versions.
Channel 0~7 are pins B8~B15.

Place ols.profile-takologic.cfg in the plugins folder of OLS client.

Based on the SLLogicLogger from Thomas Fischl.
