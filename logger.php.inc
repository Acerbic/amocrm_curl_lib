<?php
/**
 * @file
 * Функции ведения лога.
 *
 * Необходимо использовать "require_once" при подключении этого файла.
 * Использование:
 *  1. Вызовите init_logger(), чтобы указать, в какой файл писать, и дополнительные
 *  параметры настройки.
 *  2. log_event() запишет в лог ваше сообщение.
 *  3. log_event с пустыми параметрами - log_event('', '') - запишет в лог пустую строку.
 */

define('MAX_LOG_SIZE', 5*1024*1024);

$filehandles = array(); // все открытые хэндлы логов

/**
 * Инициализация логгера, проверяет размер файла и обрезает по необходимости.
 * @param  string $logname 	- имя файла для хранения логов
 * @param  boolean $log_only_file 	- если true - пишет только в файл, без вывода на экран
 * @param  boolean $debug 	- если false - не пишет в файл записи с уровнем DEBUG
 * @return boolean			- true, если успешно, иначе false.
 */
function init_logger($logname, $only_file=FALSE, $debug=TRUE) {
	global $logfile, $log_only_file, $logfile_init_size, $log_debug;

	if (empty($logname))
		return false;

    date_default_timezone_set("Europe/Moscow"); // логировать во времени по Москве

	$logfile = $logname.'.log';
	$log_only_file = $only_file;
	$log_debug = $debug;

	if (file_exists($logfile) && (filesize($logfile) > MAX_LOG_SIZE)) {
		$tmpfile = tempnam(__DIF__,'_');
		$logHandle = fopen($logfile, 'r');
		$tmpHandle = fopen($tmpfile, 'w');
		if (!$tmpHandle || !$logHandle)
			return false; //какая-то ошибка. :(

		// обрезаем до половины максимальной, чтобы не вызывать слишком часто
		fseek($logHandle, - MAX_LOG_SIZE * 0.5, SEEK_END);

		stream_copy_to_stream($logHandle, $tmpHandle);
		fclose($logHandle);
		fclose($tmpHandle);

		rename($tmpfile, $logfile);
		unlink($tmpfile);
		chmod($logfile, 0666);
	}

	clearstatcache(TRUE);

	// $logfile_init_size = (int)filesize('./'.$logfile);

	return true;
}

/**
 * Откатить файл лога к изначальному размеру (к состоянию сразу после вызова init_logger)
 */
function rollback_logger() {
	global $logfile, $logfile_init_size;
	clearstatcache(TRUE);
	@$newsize = (int) filesize('./'.$logfile); // Если файл создан после предыдущего чтения данных функцией filesize, возникает ошибка.

	log_event("rolling back $logfile : $logfile_init_size -> $newsize");

	if (is_file($logfile) && $newsize > $logfile_init_size) {
		$handle = fopen($logfile, 'r+');
		ftruncate($handle, $logfile_init_size);
		fclose($handle);
	}
}

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
	global $logfile, $log_only_file, $log_debug;

	$logName = $file;
	if ($logName == null) $logName = $logfile;
	if ($logName == null)
		return; // без логирования, если файл лога не задан.

	$filename = __DIR__.DIRECTORY_SEPARATOR.$logName;
	if (isset($filehandles[$filename])) {
		$handle = $filehandles[$filename];
	} else {
		$handle = fopen($filename, "at");
		//FIXME: Если скрипт запущен не от суперпользователя и не от создавшего файл
		//пользователя - изменять права не дозволено.
		@chmod($filename, 0666);
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

	//не пишем, если уровень DEBUG и инициализирован с настройкой не писать дебаг
	if($level == "DEBUG" && !$log_debug) {
		return false;
	}

	$dateTime = new DateTime(); // время хоста
	$sDate = $dateTime->format("[d M Y H:i:s]");

	$line = $sDate." [$level] ".$message."\r\n";

	fwrite($handle, $line);

	if ($level != "DEBUG" && !$log_only_file)
		echo $line."<br>";
}
?>
