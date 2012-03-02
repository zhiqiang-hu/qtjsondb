%modules = ( # path to module name map
    "QtJsonDb" => "$basedir/src/client",
    "QtJsonDbCompat" => "$basedir/src/clientcompat",
);
%moduleheaders = ( # restrict the module headers to those found in relative path
);
%classnames = (
    "qtjsondbversion.h" => "QtJsonDbVersion",
    "qjsondbglobal.h" => "QtJsonDbGlobal",
);
%mastercontent = (
    "core" => "#include <QtCore/QtCore>\n",
    "network" => "#include <QtNetwork/QtNetwork>\n",
);
%modulepris = (
    "QtJsonDb" => "$basedir/modules/qt_jsondb.pri",
    "QtJsonDbCompat" => "$basedir/modules/qt_jsondbcompat.pri",
);
$publicclassregexp = "QJsonDb.+";
# Module dependencies.
# Every module that is required to build this module should have one entry.
# Each of the module version specifiers can take one of the following values:
#   - A specific Git revision.
#   - any git symbolic ref resolvable from the module's repository (e.g. "refs/heads/master" to track master branch)
#
%dependencies = (
        "qtbase" => "95c5be8bc1c8f9ff8e7f95e52a8abd3cd7976ba1",
        "qtdeclarative" => "refs/heads/master",
        "qtjsbackend" => "refs/heads/master",
        "qtxmlpatterns" => "refs/heads/master",
);
