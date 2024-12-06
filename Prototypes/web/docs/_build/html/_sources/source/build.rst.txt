
Github Workflow
------------------

Skript som setter opp en virtuell maskin og dytter nyeste oppdateringene til serveren.
Deretter aktiverer workflowen et `build.sh` bash-skript (se nedenfor).

`.github/workflows/build-website.yml`

.. literalinclude:: ../../build-website.yml
   :language: yaml
   :linenos:


Bash-script
------------

Skript som restarter tjenester og sørger for at 
python er oppdatert og kjører i riktig miljø. 

`build.sh`

.. literalinclude:: ../../build.sh
   :language: bash
   :linenos:


Database
---------

Enkelt python-script som setter opp test-data til nettsiden.

`db/setup_database.py`

.. literalinclude:: ../../db/setup_database.py
   :language: python
   :linenos:

Oppstart
---------
Gunicorn kaller denne filen for å starte web-appen.

`production.py`

.. literalinclude:: ../../production.py
   :language: python
   :linenos: