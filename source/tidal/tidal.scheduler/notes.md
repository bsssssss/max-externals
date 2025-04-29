# The right time

The main challenge in the making of this scheduler is to get the timescales
right.

Tidal sends OSC messages bundled with a timestamp

This timestamp is a 64 bits number that represents the number of seconds elapsed
since 1 January 1900. The first 32 bits are the seconds and the last 32 bits are
the fraction of a second.

It represents a time (as in a date) in the future to 'play' the message at.

To schedule the message, we need to know how much time there is between the
moment the message is received and the timestamp in the future.

`delay = time_in_future - current_time` 

In C, the closest we can get to the timestamp's timescale is the UNIX time,
which tells us the number of seconds elapsed since 01 January 1970.

To convert this time to epoch 1900 we need to know how much seconds there is
in 70 years: 

`2208988800 seconds (counting leap years !)`

The convertion is a simple addition
    
`NTP_time = UNIX_time + 2208988800`

We can use this value to set a base time offset value in the instance 
that will be used to calculate the relative time of the OSC timetag.

At the object's init, we can call ```systimer_gettime()``` which gives us the
current time of max in milliseconds, divide this by 1000 to get seconds.

The base offset is simply `max_time_sec - ntp_sec` (a big negative number).

Then, when an osc message arrives, we add the timetag to the offset.
That gives us the time in the future at which point the message should be
output in seconds. Multiply this by 1000 to get the execution time in milliseconds.

---

# Schematic of the program

[RÉCEPTION DU MESSAGE OSC]
    ↓
    │ Thread Listener (tidal_scheduler_listener)
    ↓
[ANALYSE DU BUNDLE OSC]
    │ • Extraction du timetag OSC
    │ • Conversion en temps Max (osc_seconds + time_offset)
    │ • Analyse des messages contenus dans le bundle
    ↓
[AJOUT À LA FILE D'ATTENTE] ────────────>critical_enter(0)
    │ • Création d'un événement              
    │ • Stockage dans ```events[event_count]```        
    │ • Activation du scheduler si nécessaire          
    ↓                                    critical_exit(0)
    │
    │                  [ATTENTE]
    │
    ↓
[DÉCLENCHEMENT HORLOGE] ──> clock_fdelay déclenche tidal_scheduler_tick
    │
    ↓                                    critical_enter(0)
[TRAITEMENT DES ÉVÉNEMENTS]                             
    │ • Parcours de tous les événements                 
    │ • Vérification des temps d'exécution             
    │ • Exécution des événements arrivés à échéance    
    │ • Marquage comme inactifs                        
    ↓                                                  
[COMPACTAGE DU TABLEAU]                                
    │ • Déplacement des événements actifs au début     
    │ • Mise à jour du compteur d'événements           
    ↓                                                  
[PROGRAMMATION DU PROCHAIN TICK]                       
    │ • Calcul du délai jusqu'au prochain événement    
    │ • Programmation de l'horloge                     
    ↓                                    critical_exit(0)
    │
    ↓
[SORTIE DU MESSAGE]
    │ • Le message est envoyé vers la sortie via outlet_anything()
