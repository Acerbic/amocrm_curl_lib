<?php
/**
 * @file
 * Библиотека обёрток для работы с amoCRM API
 *  require_once "config.php.inc";
 */

$cookieName = null; // имя файла cookie для данного процесса.
/**
 * Проверяет код HTTP на ошибку. Прерывает выполнение в случае ошибки.
 * @param $code - код ошибки для проверки.
 * @throws Exception If код индицирует ошибоку
 */
function CheckCurlResponse($code, $out = null) {
	$code=(int)$code;
	$errors=array(
		301=>'Moved permanently',
		400=>'Bad request',
		401=>'Unauthorized',
		403=>'Forbidden',
		404=>'Not found',
		500=>'Internal server error',
		502=>'Bad gateway',
		503=>'Service unavailable'
	);
	if (isset($out) && $code == 400) {
		$out = json_decode($out, true);
		if (isset($out['response']['error']) && !empty($out['response']['error'])) {
			throw new Exception($out['response']['error'], $code);
		}
	}
	if($code!=200 && $code!=204) {
		throw new Exception(isset($errors[$code]) ? $errors[$code] : 'Undescribed error '.$code,$code);
	}
}

/**
 * Пытается авторизироваться в amoCRM.
 * @param  array $user_data	- аргументы для передачи POST вместо логина и пароля по умолчанию.
 * @param  string $subdomain - субдомен (аккаунт amoCRM)
 * @return boolean - успех операции. true если авторизация успешна.
 */
function amo_curl_authorize($user_data=null, $subdomain=AMOCRM_SUBDOMAIN) {
	global $cookieName;

	if (empty($cookieName)) {
		$cookieName = tempnam (__DIR__, "ckie_");
		register_shutdown_function('cookie_crusher');
	}

	if (is_null($user_data))
		$user_data = array('USER_LOGIN' => AMOCRM_MASTER_LOGIN,
						   'USER_HASH' => AMOCRM_MASTER_HASH);
	$link='https://'.$subdomain.'.amocrm.ru/private/api/auth.php?type=json';
	$curl=curl_init(); #Сохраняем дескриптор сеанса cURL
	#Устанавливаем необходимые опции для сеанса cURL
	curl_setopt($curl,CURLOPT_RETURNTRANSFER,true);
	curl_setopt($curl,CURLOPT_USERAGENT,'amoCRM-API-client/1.0');
	curl_setopt($curl,CURLOPT_URL,$link);
	curl_setopt($curl,CURLOPT_POST,true);
	curl_setopt($curl,CURLOPT_POSTFIELDS,http_build_query($user_data));
	curl_setopt($curl,CURLOPT_HEADER,false);
	curl_setopt($curl,CURLOPT_COOKIEFILE, $cookieName); #PHP>5.3.6 dirname(__FILE__) -> __DIR__
	curl_setopt($curl,CURLOPT_COOKIEJAR, $cookieName); #PHP>5.3.6 dirname(__FILE__) -> __DIR__
	curl_setopt($curl,CURLOPT_SSL_VERIFYPEER,0);
	curl_setopt($curl,CURLOPT_SSL_VERIFYHOST,0);

	$out=curl_exec($curl); #Инициируем запрос к API и сохраняем ответ в переменную
	$code=curl_getinfo($curl,CURLINFO_HTTP_CODE);
	curl_close($curl);

	try {
		CheckCurlResponse($code, $out);
	}
	catch(Exception $E)	{
		$err_mess = 'Ошибка: '.$E->getMessage().PHP_EOL.'Код ошибки: '.$E->getCode();
		throw new Exception($err_mess, $E->getCode());
	}

	$response=json_decode($out,true);

	#Флаг авторизации доступен в свойстве "auth"
	return isset($response['response']['auth']);
}

/**
 * Выполняет запрос POST по REST API через CURL и возвращает массив полученных данных.
 * @param  string $url - относительный адрес для запроса, например "/private/api/v2/json/leads/set"
 * @param  array $postData - данные для отправки POST
 * @param  string $subdomain - субдомен (аккаунт amoCRM)
 * @return массив присланный в ответ на запрос или null, если нет данных. В случае ошибки выполнение прерывается.
 */
function amo_curl_post($url, $postData, $subdomain=AMOCRM_SUBDOMAIN) {
	global $cookieName;

	if (empty($cookieName)) {
		$cookieName = tempnam (__DIR__, "ckie_");
		register_shutdown_function('cookie_crusher');
	}

	$link='https://'.$subdomain.'.amocrm.ru'.$url;
	$curl=curl_init(); #Сохраняем дескриптор сеанса cURL
	#Устанавливаем необходимые опции для сеанса cURL
	curl_setopt($curl,CURLOPT_RETURNTRANSFER,true);
	curl_setopt($curl,CURLOPT_USERAGENT,'amoCRM-API-client/1.0');
	curl_setopt($curl,CURLOPT_URL,$link);
	curl_setopt($curl,CURLOPT_CUSTOMREQUEST,'POST');
	curl_setopt($curl,CURLOPT_POSTFIELDS,json_encode($postData));
	curl_setopt($curl,CURLOPT_HTTPHEADER,array('Content-Type: application/json'));
	curl_setopt($curl,CURLOPT_HEADER,false);
	curl_setopt($curl,CURLOPT_COOKIEFILE, $cookieName); #PHP>5.3.6 dirname(__FILE__) -> __DIR__
	curl_setopt($curl,CURLOPT_COOKIEJAR, $cookieName); #PHP>5.3.6 dirname(__FILE__) -> __DIR__
	curl_setopt($curl,CURLOPT_SSL_VERIFYPEER,0);
	curl_setopt($curl,CURLOPT_SSL_VERIFYHOST,0);

	$out=curl_exec($curl); #Инициируем запрос к API и сохраняем ответ в переменную
	$code=curl_getinfo($curl,CURLINFO_HTTP_CODE);
	curl_close($curl);
	try {
		CheckCurlResponse($code, $out);
	}
	catch(Exception $E)	{
		$err_mess = 'amo_curl_post ошибка: '.$E->getMessage().PHP_EOL.'Код ошибки: '.$E->getCode();
		throw new Exception($err_mess, $E->getCode());
	}

	$Response=json_decode($out,true);
	return $Response['response'];
}

/**
 * Выполняет запрос GET по REST API через CURL и возвращает массив полученных данных.
 * @param  string $url - (!) относительный адрес для запроса.
 * @param  array $args - данные для отправки GET.
 * @param  long $modifiedSince - метка времени для передачи в виде параметра modified-since.
 * @param  string $subdomain - субдомен (аккаунт amoCRM).
 * @return часть ['response'] массива присланного в ответ на запрос или null, если нет данных. В случае ошибки выполнение прерывается.
 */
function amo_curl_get($url, $args=null, $modifiedSince=null, $subdomain=AMOCRM_SUBDOMAIN) {
	global $cookieName, $server_time;
	if (empty($cookieName)) {
		$cookieName = tempnam (__DIR__, "ckie_");
		register_shutdown_function('cookie_crusher');
	}

	$curl=curl_init(); #Сохраняем дескриптор сеанса cURL
	#Устанавливаем необходимые опции для сеанса cURL
	curl_setopt($curl,CURLOPT_RETURNTRANSFER,true);
	curl_setopt($curl,CURLOPT_USERAGENT,'amoCRM-API-client/1.0');
	curl_setopt($curl,CURLOPT_HEADER,false);
	if (!is_null($modifiedSince)) {
		$dateTime = new DateTime(); // время хоста
		$dateTime->setTimestamp($modifiedSince); // время по метке
        $dateTime->setTimezone(new DateTimeZone(AMOCRM_ACCOUNT_TIMEZONE));
		$sDate = $dateTime->format("D, d M Y H:i:s");
		curl_setopt($curl, CURLOPT_HTTPHEADER, array("if-modified-since: $sDate"));
	}
	curl_setopt($curl,CURLOPT_COOKIEFILE, $cookieName);
	curl_setopt($curl,CURLOPT_COOKIEJAR, $cookieName);
	curl_setopt($curl,CURLOPT_SSL_VERIFYPEER,0);
	curl_setopt($curl,CURLOPT_SSL_VERIFYHOST,0);


	$retry_counter = 3; $completion_flag = false;
	if (is_null($args)) $args = array();

	while ($retry_counter && !$completion_flag) {

		$completion_flag = true; // надежда...

		$link = 'https://'.$subdomain.'.amocrm.ru'.$url.'?'.http_build_query($args);
		curl_setopt($curl,CURLOPT_URL,$link);

		$out=curl_exec($curl); #Инициируем запрос к API и сохраняем ответ в переменную
		$code=curl_getinfo($curl,CURLINFO_HTTP_CODE);

		try {
			CheckCurlResponse($code, $out);
		}
		catch(Exception $E)	{
			$err_mess = 'Ошибка: '.$E->getMessage().PHP_EOL.'Код ошибки: '.$E->getCode();
			$completion_flag = false; // .. надежда не оправдалась
			$retry_counter--;
			sleep(1);
			continue; //while ($retry_counter && !$completion_flag)
		}

		$response=json_decode($out,true);
		$chunk = $response['response'];
		if (!empty ($chunk['server_time']) )
			$server_time = $chunk['server_time'];
	}

	curl_close($curl);
	if (!$completion_flag) {
		throw new Exception("Unrecoverable error during amo_curl_get");
	}
	return $chunk;
}

/**
 * Массовое получение данных.
 * @param  string $url 		- (!) относительный адрес для запроса.
 * @param  string $dataType - имя запрашиваемых данных ('leads', 'contacts'...) для использования многостраничного запроса (limit_rows и limit_offset).
 * @param  array $args 		- данные для отправки GET. Если указан аргумент 'limit_rows', то он обрабатывается без ограничения в 500 элементов,  описаного в API.
 * @param  int $modifiedSince - ограничение по дате (по времени amoCRM)
 * @param  string $processor  - имя callback для обработки единичного элемента, если указан - то в конечный результат включается результат обработки, а не исходный элемент.
 *                            	Возвращайте NULL, если элемент не нужно включать в результаты обработки.
 * @param  string $subdomain - иной аккаунт, чем указаный в конфиге
 * @return array            - {data => {id => {element}...}, 'count' => TOTAL}, если существует $element['id'], иначе
 *                            {data => [{element}], 'count' => TOTAL}
 * значение TOTAL - подсчитанное число элементов, полученных в ответ из amoCRM - МОЖЕТ БЫТЬ БОЛЬШЕ, чем реальное число элементов в data
 */
function amo_curl_get_mass($url, $dataType, $args=null, $modifiedSince=null, $processor=null, $subdomain=AMOCRM_SUBDOMAIN) {
	global $server_time;

	$args = is_null($args)? array() : $args;
	$nItemsToGet = isset($args['limit_rows'])? $args['limit_rows'] : 0;
	$limitRows = $nItemsToGet && $nItemsToGet>0;
	$originalOffset = isset($args['limit_offset'])? $args['limit_offset']: 0;

	$resultData = array();
	$args['limit_offset'] = $originalOffset;
	$actualCounter = 0;

	do {
		$args['limit_rows'] = ($limitRows)? min($nItemsToGet, 400) : 400;
		$batch = amo_curl_get($url, $args, $modifiedSince, $subdomain);

		if ($batch != null) {
			foreach ($batch[$dataType] as $element) {
				// Обработаем элемент, если требуется
				$processed_element = (!empty($processor) && function_exists($processor))? $processor($element) : $element;

				// Сохраняем только непустые результаты обработки. Ключ по ID, если есть, или по порядку.
				if(!is_null($processed_element)) {
					if (isset($element['id']))
						$resultData[$element['id']] = $processed_element;
					else
						$resultData[] = $processed_element;
				}
			}
			$elementsRecieved = count($batch[$dataType]);
		} else {
			$elementsRecieved = 0;
		}

		$actualCounter += $elementsRecieved;
		$nItemsToGet -= $elementsRecieved;
		$args['limit_offset'] += 400;
	} while ($elementsRecieved == 400 && !($limitRows && $nItemsToGet<=0));

	return array('data' => $resultData, 'count' => $actualCounter, 'server_time' => $batch['server_time']);
}

/**
 * массовый вариант amo_curl_post().
 * @param  string $url - (!) относительный адрес для запроса
 * @param  array $postData - данные для отправки POST
 * @param  string $subdomain - субдомен (аккаунт amoCRM)
 * @return массив присланный в ответ на запрос или null, если нет данных. В случае ошибки выполнение прерывается.
 */
function amo_curl_post_mass($url, $postData, $subdomain=AMOCRM_SUBDOMAIN) {
	if (!isset($postData) || !isset($postData['request'])) {
		return null;
	}
	$count = 0;
	$response = array();
	$request = array();

	$datatype = array_keys($postData['request']);
	$datatype = $datatype[0];
	$methods = array_keys($postData['request'][$datatype]);
	foreach ($methods as $method) {
		foreach($postData['request'][$datatype][$method] as $item) {
			$count++;
			$request['request'][$datatype][$method][] = $item;
			if ($count == 500) {
				$count = 0;
				$response = array_merge_recursive(amo_curl_post($url, $request, $subdomain), $response);
				$request = array();
			}
		}
	}
	// остаток (те, что не добрали до 500)
	if (count($request) > 0) {
		$response = array_merge_recursive(amo_curl_post($url, $request), $response);
	}
	return (count($response) > 0)? $response : null;
}

/**
 * Удалить сущность из amoCRM
 * @param  String $entity_name
 *   Название сущности. Допустимые значение - 'contact', 'deal'
 * @param  array $ids
 *   Набор идентификаторов сущностей для удаления
 * @param  string $subdomain
 *   субдомен (аккаунт amoCRM)
 * @return boolean               TRUE, если не произошло ошибок. NULL иначе.
 *   Ошибки CURL создают исключение
 */
function amo_curl_delete($entity_name, $ids, $subdomain=AMOCRM_SUBDOMAIN) {
	global $cookieName;

	// TODO уточнить (протестировать!) пути для других сущностей
	$request_urls = array(
		'contact' => 'private/api/contact_delete.php',
		'deal' => 'private/api/deal_delete.php'
	);

	$url = $request_urls[$entity_name];
	if (empty($url) || count($ids) == 0) {
		return NULL;
	}

	$link = "https://$subdomain.amocrm.ru/$url?ACTION=DELETE&"
		. http_build_query(array('ID' => $ids));

	$curl=curl_init(); #Сохраняем дескриптор сеанса cURL
	#Устанавливаем необходимые опции для сеанса cURL
	curl_setopt($curl,CURLOPT_RETURNTRANSFER,true);
	curl_setopt($curl,CURLOPT_USERAGENT,'amoCRM-API-client/1.0');
	curl_setopt($curl,CURLOPT_URL,$link);
	curl_setopt($curl,CURLOPT_HEADER,false);
	curl_setopt($curl,CURLOPT_COOKIEFILE,$cookieName);
	curl_setopt($curl,CURLOPT_COOKIEJAR,$cookieName);
	curl_setopt($curl,CURLOPT_SSL_VERIFYPEER,0);
	curl_setopt($curl,CURLOPT_SSL_VERIFYHOST,0);
	curl_setopt($curl,CURLOPT_CUSTOMREQUEST,'POST');
	$result = curl_exec($curl); #Инициируем запрос к API и сохраняем ответ в переменную
	curl_close($curl);

	try {
		$sxe = new SimpleXMLElement($result);
		if (isset($sxe->result) && (String)$sxe->result == 'true') {
			return TRUE;
		}
	} catch (Exception $e) {
		// возвращён не XML документ
	}
	return NULL;
}

/*
 * name: amo_curl_upload_file - загрузка файла
 * @param integer $element_id - ID контакта/сделки, к которому прикрепить файл.
 * @param integer $element_type - тип элемента. 1 - контакт, 2 - сделка
 * @param string $filename - путь до файла.
 * @param string $subdomain - субдомен (аккаунт amoCRM)
 * @return boolean - TRUE, если не произошло ошибок. NULL иначе.
 *   Ошибки CURL создают исключение
 */

function amo_curl_upload_file($element_id, $element_type, $filename, $subdomain=AMOCRM_SUBDOMAIN) {
	global $cookieName;

	//Загрузка файла
	$link = "https://".$subdomain.".amocrm.ru/private/notes/edit2.php?ACTION=ADD_NOTE&v=2&ID={$element_id}&ELEMENT_TYPE={$element_type}";
	$curl=curl_init();
	curl_setopt($curl,CURLOPT_RETURNTRANSFER,true);
	curl_setopt($curl,CURLOPT_USERAGENT,'amoCRM-API-client/1.0');
	curl_setopt($curl,CURLOPT_URL,$link);
	curl_setopt($curl,CURLOPT_HEADER,false);
	curl_setopt($curl,CURLOPT_COOKIEFILE,$cookieName);
	curl_setopt($curl,CURLOPT_COOKIEJAR,$cookieName);
	curl_setopt($curl,CURLOPT_SSL_VERIFYPEER,0);
	curl_setopt($curl,CURLOPT_SSL_VERIFYHOST,0);
	curl_setopt($curl,CURLOPT_CUSTOMREQUEST,'POST');
	curl_setopt($curl,CURLOPT_POST,1);
	curl_setopt($curl,CURLOPT_POSTFIELDS,array('UserFile' => "@".$filename));
	$result = curl_exec($curl);
	curl_close($curl);

	//Проверка загрузки, парсинг имени файла на сервере.
	if (strpos($result, '<script type="text/javascript">') >= 0 && strpos($result, 'Notes.add') >= 0) {
		$file_link = substr($result, strpos($result, 'link: ')+7);
		$file_link = trim(substr($file_link, 0, strpos($file_link, 'text: ')));
		$file_link = substr($file_link, 0, strlen($file_link)-2);
	}
	else {
		throw new Exception("Failed to upload file in amo_curl_upload_file");
	}

	//Создание заметки типа "файл"
	$args = array(
		'ID' => $element_id,
		'ACTION' => 'ADD_NOTE',
		'ATTACH' => $file_link,
		'BODY' => basename($filename),
		'ELEMENT_TYPE' => 2
	);

	$curl=curl_init();
	curl_setopt($curl,CURLOPT_RETURNTRANSFER,true);
	curl_setopt($curl,CURLOPT_USERAGENT,'amoCRM-API-client/1.0');
	curl_setopt($curl,CURLOPT_URL,$link);
	curl_setopt($curl,CURLOPT_HEADER,false);
	curl_setopt($curl,CURLOPT_COOKIEFILE,$cookieName);
	curl_setopt($curl,CURLOPT_COOKIEJAR,$cookieName);
	curl_setopt($curl,CURLOPT_SSL_VERIFYPEER,0);
	curl_setopt($curl,CURLOPT_SSL_VERIFYHOST,0);
	curl_setopt($curl,CURLOPT_CUSTOMREQUEST,'POST');
	curl_setopt($curl,CURLOPT_POST,1);
	curl_setopt($curl,CURLOPT_POSTFIELDS,$args);
	$result = curl_exec($curl);
	curl_close($curl);

	$result = json_decode($result);

	if ($result->status != 'ok') {
		throw new Exception("Failed to upload file in amo_curl_upload_file");
	}
	return true;
}

/**
 * Функция вызывается автоматически при завершении скрипта и удаляет
 * файл с печеньками.
 */
function cookie_crusher() {
	global $cookieName;

	if (!empty($cookieName))
		unlink($cookieName);
}
?>
