CPMProcWx
=========================================================

    To include wxWidgets into CPMProcWx...

        - In src/CPMProcWx.mpc, uncomment...

            macros += CPMPROCESS_WX

            macros += wxHAS_MODE_T // If the application uses ACE or another library that defines mode_t

            unicode = 1  // if wxWidgets is unicode

        - In features file CPMPROCWX_PRJ_ROOT/features, add this line:
            wxwindows=1

        - Set env vars
            CPMPROCWX_ABC_PORT = 10199 (10199 is an example)

        - Generate project/solution/makefile (dpo.pl)

            - Add wxcore_dpo.mpb as dependency (or more)

        - When using CPMProcWx with CPMM, defines port as "$(CPMPROCWX_PORT)" in cpm_*.cfg
          (to be coherent with gui)

        - Generate project/solution/makefile with dpo.pl
            (if dpo.pl is already running, restart it. Each time we
             edit the features file, we need to orestart dpo.pl).


