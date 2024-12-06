NGINX-konfigurasjon
===================

Slik ser nginx-konfigurasjonen ut.

.. literalinclude:: ../../nginx/default
   :language: nginx
   :linenos:

Forklaring:
-----------
**upstream**
   - Ruter websocketen korrekt i henhold til gunicorn.
**server**:
   - Definerer en ny virtuell server.
**location /**:
   - Håndterer forespørsler til rot-URL-en.