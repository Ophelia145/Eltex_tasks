Написала модули ядра, которые взаимодействуют с системой через символьное устройство /dev, файловую систему /proc и файловую систему /sys

во всех реализованы операции чтения и записи

Вывод сборки:  
![alt text](image.png)

Загружаем модуль:  
sudo insmod str_into_logs.ko  
![alt text](image-1.png)  
fine vm it is  
make  
![alt text](image-2.png)  
sudo insmod str_into_logs.ko  
sudo dmesg | tail  
![alt text](image-3.png)  
  
собрали и загрузили модули dev,sys,proc  
Логи:  
![alt text](image-4.png)  

Проверяем chrdev   
![alt text](image-5.png)  
строка записана и прочтена   
  
проверяем модуль proc  
![alt text](image-6.png)  

sys  
![alt text](image-7.png)  
  
Выгрузила модули и проверила логи  
![alt text](image-8.png)   
