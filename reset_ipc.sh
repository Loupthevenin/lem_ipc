#!/bin/bash

SHM_KEY=0x1234
SEM_KEY=0x1337

echo "🧹 Cleaning shared memory and semaphores..."

# Supprimer la mémoire partagée
ipcrm -M $SHM_KEY 2>/dev/null && echo "✅ Shared memory removed." || echo "⚠️  No shared memory to remove."

# Supprimer le sémaphore
ipcrm -S $SEM_KEY 2>/dev/null && echo "✅ Semaphore removed." || echo "⚠️  No semaphore to remove."
