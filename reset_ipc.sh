#!/bin/bash

SHM_KEY=0x1234
SEM_KEY=0x1337
MSG_KEY=0x1558

echo "🧹 Cleaning shared memory and semaphores..."

# Supprimer la mémoire partagée
ipcrm -M $SHM_KEY 2>/dev/null && echo "✅ Shared memory removed." || echo "⚠️  No shared memory to remove."

# Supprimer le sémaphore
ipcrm -S $SEM_KEY 2>/dev/null && echo "✅ Semaphore removed." || echo "⚠️  No semaphore to remove."

# Supprimer le MSGQ
ipcrm -Q $MSG_KEY 2>/dev/null && echo "✅ MSGQ removed." || echo "⚠️  No msgq to remove."
