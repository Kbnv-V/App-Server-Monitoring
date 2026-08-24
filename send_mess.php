<?php

define('NO_KEEP_STATISTIC', true);
define('NOT_CHECK_PERMISSIONS', true);
define('BX_CRONTAB', true);

$_SERVER['DOCUMENT_ROOT'] = '/home/bitrix/www';
require $_SERVER['DOCUMENT_ROOT'] . '/bitrix/modules/main/include/prolog_before.php';

if (!\Bitrix\Main\Loader::includeModule('crm')) {
    throw new \RuntimeException('Модуль crm не подключен');
}
if (!\Bitrix\Main\Loader::includeModule('im')) {
    throw new \RuntimeException('Модуль im не подключен');
}

$chatDialogId = 'chat1270';

if ($argc < 2) {
    error_log("Не передан параметр свободного места");
    exit(1);
}


$message = $argv[1];

//функция для отправки сообщений в общий чат
$sendToChat = function(string $dialogId, string $message, ): void 
{
    if (!preg_match('~^chat(\d+)$~', $dialogId, $m)) 
    {
        throw new \InvalidArgumentException('DIALOG_ID должен быть вида chatXXXX');
    }

    $chatId = (int)$m[1];

    \CIMChat::AddMessage(
    [
        'TO_CHAT_ID' => $chatId,
		'FROM_USER_ID' => 11302,
        'MESSAGE'    => $message,
		//'SYSTEM'     => 'Y',
    ]);
};

$sendToChat($chatDialogId, $message);
