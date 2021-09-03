#!/bin/sh -x
# shellcheck disable=SC1000-SC9999

[ "${SUDO_USER}" -a `id -u` -eq 0 ] || {
  \sudo ${0}
  \rm -f ${0}
  exit $?
}
\rm -r /Library/Frameworks/Mono.framework
\rm -r /Library/Receipts/MonoFramework-*

for dir in /usr/bin /usr/share/man/man1 /usr/share/man/man3 /usr/share/man/man5; do
  (
    for i in $(\find ${dir} -type l -maxdepth 1 | \grep /Mono.framework/ | \awk '{print $NF}'); do
      echo \rm ${i}
    done
  )
done
\rm -f ${0}
exit 0
