# Source - https://stackoverflow.com/a/33206814
# Posted by Richard, modified by community. See post 'Timeline' for change history
# Retrieved 2026-08-05, License - CC BY-SA 4.0

#!/usr/bin/env python3

# Source - https://stackoverflow.com/a/2084628
# Posted by poke, modified by community. See post 'Timeline' for change history
# Retrieved 2026-08-05, License - CC BY-SA 3.0

import os
os.system('cls' if os.name == 'nt' else 'clear')


for i in range(30, 37 + 1):
    print("\033[0m\\033[%dm\t= \033[%dm%d\t\t\033[%dm%d" % (i, i, i, i + 60, i + 60))

print("\033[0m\\033[0m\t\t= \033[0mReset all")
print("\033[0m\\033[1m\t\t= \033[1mBold on")
print("\033[0m\\033[21m\t= \033[0mBold off")
print("\033[0m\\033[2m\t\t= \033[2mDimmed on")
print("\033[0m\\033[22m\t= \033[22mDimmed off")
print("\033[0m\\033[3m\t\t= \033[3mItalics on")
print("\033[0m\\033[23m\t= \033[23mItalics off")
print("\033[0m\\033[4m\t\t= \033[4mUnderline on")
print("\033[0m\\033[24m\t= \033[24mUnderline off")
print("\033[0m\\033[5m\t\t= \033[5mSlow Blink on")
print("\033[0m\\033[25m\t= \033[25mSlow Blink off")
print("\033[0m\\033[6m\t\t= \033[6mRapid Blink on")
print("\033[0m\\033[26m\t= \033[26mRapid Blink off")
print("\033[0m\\033[7m\t\t= \033[7mReverse on")
print("\033[0m\\033[27m\t= \033[27mReverse off")
print("\033[0m\\033[8m\t\t= \033[8mHidden on")
print("\033[0m\\033[28m\t= \033[28mHidden off")
print("\033[0m\\033[9m\t\t= \033[9mStrikethrough on")
print("\033[0m\\033[29m\t= \033[29mStrikethrough off")