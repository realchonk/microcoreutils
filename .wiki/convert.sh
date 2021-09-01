#!/bin/sh

while read line; do
   [ '## Programs' = "${line}" ] && break
done

read _
read _

while read line; do
   [ -z "${line}" ] && break
   link="$(echo "${line}" | cut -d'|' -f2)"
   finished="$(echo "${line}" | cut -d'|' -f3)"
   man="$(echo "${line}" | cut -d'|' -f4)"
   notes="$(echo "${line}" | cut -d'|' -f5)"
   
   if [ " ✔ " = "${man}" ]; then
      name="$(echo "${link}" | sed ' s/^\s\+\[\([a-z]\+\).*/\1/')"
      manlink="https://stuerz.xyz/mc-${name}.1.html"
      echo "|${link}|${finished}| [✔](${manlink}) |${notes}|"
   else
      echo "|${link}|${finished}|${man}|${notes}|"
   fi
done
