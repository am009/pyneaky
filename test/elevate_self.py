# requires admin privilege
import sys
sys.path.append(r"C:\Users\warren\d\pyNeaky\pyneaky\build\lib.win-amd64-3.8")

import neaky

print(neaky.get_username())
neaky.elevate_thread()
print(neaky.get_username()) # SYSTEM
