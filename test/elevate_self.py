# requires admin privilege
import sys
sys.path.append(r"C:\Users\warren\d\pyNeaky\pyneaky\build\lib.win-amd64-3.9")

import neaky

print(neaky.get_username())
neaky.elevate_thread()
print(neaky.get_username()) # SYSTEM
