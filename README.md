Projeto Final -FSE 2020.1 - UnB - FGA
=========================
Joao Gabriel Rossi de Borba  - 17/0013693

Ivan Diniz Dobbin - 17/0013278

#

## Como rodar o servidor central

1. Faça clone deste repositório e acesse a pasta da raspberry pi com o comando ```$ cd rasp/```;
2. Compile o servidor central com o comando ```$ make```;
3. Execute com o comando ```$ ./bin/bin```;

#

## Como rodar os clientes distribuidos
1. Faça clone deste repositório e acesse a pasta dos clientes com o comando ```$ cd esp32/```;
2. Faça a build do programa com o comando  `$ idf.py build`;
3. Utilize o comando `$ idf.py -p {PORT} flash monitor` para copiar o programa na esp;
