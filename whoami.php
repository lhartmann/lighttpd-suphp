<?php
// This is a test script for suphp.
// Place it in a target user's public_html and in the main site directory.
// Read both through the browser and check the different responses to whoami.

header('Content-type: text/plain');

system('whoami');
system('pwd');
