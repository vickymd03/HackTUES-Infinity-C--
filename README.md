# HackTUES-Infinity-C--
Отбор: C--

Проект:GreenBo

Първоначалната ни идея е да направим самоходна машина за изследване на чужди планети с помощта на радиоуправление. За целта ще разработим уеб приложение, представящо табло за управление, чрез което ще се издават команди към машината и ще се визуализира информацията на получените от нея данни. GreenBo ще има четири независими завиващи колела, роботизирана ръка, слънчев панел, видео камера, захранващ блок, изчислителен блок и датчици.

За разработването на хардуера ще се използват STM32H7, видео камера OV7725, ESP32-WROOM и управляваща електроника. Задвижването ще се извършва чрез серво мотори MG996R. Механиката ще бъде частично принтирана на 3D принтер, за останалото ще се използват плексигласови плоскости и други материали. Идеята е електрониката да предава изображения от камерата 1 или 2 пъти в секунда, а по втори поток да изпраща данни от приборите и да получава команди за управлението.
