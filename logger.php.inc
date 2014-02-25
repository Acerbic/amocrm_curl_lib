<?php
/*
Функции ведения лога. 
Необходимо использовать "require_once" при подключении этого файла
*/

$filehandles = array(); // все открытые хэндлы логов

/**
 * Записать сообщение в файл и на экран (html вывод). Имя файла определяется глобальной переменной $logfile или передаётся аргументом
 * @param  string $message текст сообщения.
 * @param  string $level   "уровень тревожности" сообщения. Если $level и $message - оба пустые, 
 * то будет выведена в файл пустая строка для декорации. Если $level == "DEBUG", то вывод на экран будет пропущен.
 * @param  string $file    имя файла. Если null, то используется глобальная переменная $logfile
 */
function log_event($message, $level="INFO", $file=null) {

	//открываем нужный файл
	global $filehandles;
	global $logfile;

	$logName = $file;
	if ($logName == null) $logName = $logfile;
	if ($logName == null) 
		return; // без логирования, если файл лога не задан.

	$filename = __DIR__.DIRECTORY_SEPARATOR.$logName.".log";
	if (isset($filehandles[$filename])) {
		$handle = $filehandles[$filename];
	} else {
		$handle = fopen($filename, "at");
		$filehandles[$filename] = $handle;
	}

	if ($handle === false || $handle === null)
		return false;

	//теперь пишем сообщение.

	//пустая строка в лог-файл
	if (empty($message) && empty($level)) {
		fwrite($handle, "\r\n");
		return;
	}

	$dateTime = new DateTime(); // время хоста
	$sDate = $dateTime->format("[d M Y H:i:s]");

	$line = $sDate." [$level] ".$message."\r\n";
	fwrite($handle, $line);
	if ($level != "DEBUG")
		echo $line."<br>";
}
?>