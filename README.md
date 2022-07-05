# projeto-1-concorrente
Disciplina INE5410 da UFSC  
Simulação do Restaurante Universitário, código base de Bruno Dourado Miranda  
Nota avaliada: 9,0  
Problemas:
- Uso de mutex para sincronização (em pthreads isso possui comportamento indefinido, então o ideal seria trocar para semáforos)
- Espera ocupada na função para o student se sentar (ideal seria utilizar semáforos assim como os que controlam as comidas nas bácias)
