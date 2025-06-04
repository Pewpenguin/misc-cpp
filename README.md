# Miscellaneous CPP code

Just a bunch of random C++ stuff I wrote for no real reason.  

## Warning: `crash.cpp` May Cause System Instability

The file [`crash.cpp`](./crash.cpp) demonstrates the use of **undocumented Windows internal APIs** to raise a hard system error, which can lead to:

- A **blue screen of death (BSOD)**
- **Immediate system shutdown**
- Behavior flagged by antivirus or endpoint protection software

> **Do NOT run this file on production or personal systems with unsaved work.**
>
> It is provided strictly for **educational and research purposes**.