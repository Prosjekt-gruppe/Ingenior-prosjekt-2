Systemd-tjenester
==================

Gunicorn
--------
For å kjøre flask-appen.


.. literalinclude:: ../../services/gunicorn.service
   :language: ini
   :linenos:


PyMQTT
------
For å kjøre Python MQTT-klienten på WebServeren.

.. literalinclude:: ../../services/pymqtt.service
   :language: ini
   :linenos: