Jinja
----------------

Flask bruker `Jinja` og `templates` til å konstruere nettsiden. 
Ved å velge en slik løsning kan man autogenerere HTML-kode ved hjelp av flask-serveren.
Følgende HTML-kode blir satt sammen av flask-applikasjonen når nettsiden laster
og setter dem sammen korrekt.

**base.html**

.. literalinclude:: ../../app/templates/base.html
   :language: html
   :linenos:

**front.html**

.. literalinclude:: ../../app/templates/front/fpage.html
   :language: html
   :linenos:
