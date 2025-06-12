# new approach

The new approach is promising, but it is unclear how the register operations should be treated.

- Loads from memory locations, that are not asociated with the marked variables are not going to be ignored.
- Now let's assume two foreign values are loaded into registers. Is it worth marking these registers as unusable then?
- I think this could be the key, if the value is loaded into the register, and this value is asociated with a foreign memory
address the register is to be "emptied".
