CPMFoo
=========================================================

    To include wxWidgets into CPMFoo...

        - In src/CPMFoo.mpc, uncomment...

            macros += CPMPROCESS_WX

            macros += wxHAS_MODE_T // If the application uses ACE or another library that defines mode_t

            unicode = 1  // if wxWidgets is unicode

        - In features file CPMFOO_PRJ_ROOT/features, add this line:
            wxwindows=1

        - Set env vars
            CPMFOO_PORT = 10199 (10199 is an example)

        - Generate project/solution/makefile (dpo.pl)

            - Add wxcore_dpo.mpb as dependency (or more)

        - When using CPMFoo with CPMM, defines port as "$(CPMFOO_PORT)" in cpm_*.cfg
          (to be coherent with gui)

        - Generate project/solution/makefile with dpo.pl
            (if dpo.pl is already running, restart it. Each time we
             edit the features file, we need to orestart dpo.pl).


