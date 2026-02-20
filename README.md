Il flusso dalla A alla Z:

main valida gli argomenti
init_rules fa il parsing e inizializza tutti i mutex
init_philos alloca i filosofi e assegna le forchette con gerarchia
main lancia i thread dei filosofi con philo_routine
main lancia il thread monitor con monitor_routine
Ogni filosofo ciclicamente: mangia → dorme → pensa
Il monitor ogni 500µs controlla morti e pasti completati
Quando il monitor rileva la fine, setta rules->dead = 1 e termina
I filosofi si accorgono del flag e terminano da soli
main joina tutti i thread e chiama cleanup

I tre mutex e a cosa servono:
MutexProteggeforks[i]accesso esclusivo alle forchette fisicheprint_mutexstampa non sovrapposta tra threaddeath_mutexlettura/scrittura del flag rules->deadmeal_mutextime_of_last_meal e meals_eaten di ogni filosofo