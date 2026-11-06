# Veille 2DS (SleepButton)

Petit homebrew pour 2DS / 3DS sous Luma3DS, avec deux fonctions :

1. **Veille reelle** : demande la veille au systeme (service `ptm:sysm`).
   Le reveil par un autre bouton que le slider est a tester (voir plus bas).
2. **Ecran eteint (economie)** : coupe le retroeclairage des deux ecrans.
   N'importe quel bouton ou toucher de l'ecran rallume (sauf HOME et POWER).

Commandes : Haut/Bas + A, ou toucher une ligne sur l'ecran du bas. START quitte.

## Obtenir le .3dsx (sans rien installer sur ton PC)

1. Cree un compte GitHub gratuit, puis un depot (par exemple `sleepbutton`).
2. Envoie-y tout le contenu de ce dossier (bouton "Add file" > "Upload files").
   Le dossier cache `.github/workflows/` doit etre envoye aussi.
3. Onglet **Actions** : la compilation demarre toute seule (ou "Run workflow").
4. Quand elle est verte, ouvre l'execution, descends a **Artifacts** et
   telecharge `SleepButton` (un zip contenant `SleepButton.3dsx`).

## Installation

Copie `SleepButton.3dsx` dans `/3ds/SleepButton/` sur la carte SD
(cree le dossier `SleepButton`, puis mets le fichier dedans, nom `SleepButton.3dsx`).
Lance-le depuis le Homebrew Launcher / Grid Launcher.

## Test du reveil (le point critique)

1. Lance l'appli, choisis **Veille reelle**.
2. Si la console s'endort, essaie de la reveiller avec : bouton POWER,
   puis bouton HOME. Note ce qui marche.
3. Si seul le slider reveille la console, la veille reelle ne te sert a rien :
   utilise le mode **Ecran eteint**.

Si l'ecran du haut affiche "ptm:sysm indisponible" ou un code `RequestSleep`
different de `0x00000000`, note le texte exact.

## Compiler en local (optionnel)

Avec devkitPro (devkitARM + libctru + 3ds-tools) installe : `make`.
