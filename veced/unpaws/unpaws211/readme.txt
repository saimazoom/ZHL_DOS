Unpaws 2.1.1
============

This is a small fix to get a correct space when is
formatting the final text of the game, the problem
is that never enter to the sentence for operator
priority.

now it's fixed in version 2.1.1

2007, German Gonzalez
www.devweb.cl


diff patch:
------------------------------------------------------------------------------------

530c530
<           else if not (c in[6,13,16,17,18,19,20,21,22,23]) then inc(XPos);
---
>           else if not c in[6,13,16,17,18,19,20,21,22,23] then inc(XPos);

------------------------------------------------------------------------------------





