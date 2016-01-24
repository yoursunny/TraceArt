<?php
if (@$_SERVER['HTTP_ORIGIN'] != 'https://traceart.yoursunny.cn') {
  header('HTTP/1.1 403');
  die;
}
header('Access-Control-Allow-Origin: https://traceart.yoursunny.cn');

define('ALLOCFILE', '/home/traceart/alloc.txt');
define('FLATFILE', '/home/traceart/flat.txt');
define('REVCONF', '/home/traceart/db.traceart');
define('FWDCONF', '/home/traceart/db.traceart.yoursunny.cn');

$input = file_get_contents('php://input');

$pAlloc = proc_open('python3 alloc.py '.ALLOCFILE,
                    array(
                      0=>array('pipe', 'r'),
                      1=>array('pipe', 'w')
                    ),
                    $pipes);
fwrite($pipes[0], $input);
fclose($pipes[0]);
$alloc = stream_get_contents($pipes[1]);
fclose($pipes[1]);
proc_close($pAlloc);
file_put_contents(ALLOCFILE, $alloc);

$pTransform = proc_open('python3 transform.py ',
                        array(
                          0=>array('file', ALLOCFILE, 'r'),
                          1=>array('file', FLATFILE, 'w')
                        ),
                        $pipes);
proc_close($pTransform);

$pBindconf = proc_open('python3 bindconf.py ',
                       array(
                         0=>array('file', FLATFILE, 'r'),
                         1=>array('file', REVCONF, 'w')
                       ),
                       $pipes);
proc_close($pBindconf);

$pForwardconf = proc_open('python3 forwardconf.py ',
                          array(
                            0=>array('file', ALLOCFILE, 'r'),
                            1=>array('file', FWDCONF, 'w')
                          ),
                          $pipes);
proc_close($pForwardconf);

$pLastname = proc_open('python3 lastname.py',
                       array(
                         0=>array('file', ALLOCFILE, 'r'),
                         1=>array('pipe', 'w')
                       ),
                       $pipes);
$lastname = stream_get_contents($pipes[1]);
fclose($pipes[1]);
proc_close($pLastname);
file_put_contents(ALLOCFILE, $alloc);

exec('./bind-reload');

echo $lastname;
?>
