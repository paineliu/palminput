mkdir %USERPROFILE%\PalmInput
attrib +h %USERPROFILE%\PalmInput
cacls "%USERPROFILE%\PalmInput" /e /t /g "ALL APPLICATION PACKAGES:c"
