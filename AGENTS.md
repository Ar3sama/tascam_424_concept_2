# JUCE Plugin Template

Ce repo est un template neutre pour creer des plugins audio JUCE. Le code ajoute doit rester clair, simple, maintenable et temps reel safe.

## Structure

```txt
source/
|-- dsp/
|   |-- PluginDSP.h
|   `-- PluginDSP.cpp
|-- parameters/
|   |-- ParameterIDs.h
|   |-- ParameterLayout.h
|   `-- ParameterLayout.cpp
|-- ui/
|   `-- lookandfeel/
|       |-- PluginLookAndFeel.h
|       `-- PluginLookAndFeel.cpp
|-- PluginConfig.h
|-- PluginEditor.cpp
|-- PluginEditor.h
|-- PluginProcessor.cpp
`-- PluginProcessor.h
```

## Responsabilites

- `PluginProcessor` : coordination avec JUCE/le host, bus audio, APVTS, etat du plugin, appel du DSP.
- `PluginDSP` : traitement audio uniquement. Ajouter ici les filtres, delays, compresseurs, oscillateurs, saturations, etc.
- `parameters/` : IDs et declaration des parametres APVTS.
- `PluginEditor` : interface utilisateur, layout, composants, attachments APVTS.
- `PluginLookAndFeel` : couleurs et style UI centralises.
- `PluginConfig.h` : constantes simples du template, tailles et marges.
- `images/` : toutes les ressources graphiques.

Ne pas melanger UI et DSP.

## Parametres

Utiliser `AudioProcessorValueTreeState` pour tous les parametres.

Pour ajouter un parametre :
1. ajouter son ID dans `ParameterIDs.h`
2. l'enregistrer dans `ParameterLayout.cpp`
3. le lire de maniere temps reel safe dans le Processor ou le DSP
4. connecter l'UI avec un attachment JUCE (`SliderAttachment`, `ButtonAttachment`, etc.)

Le template contient deja un parametre global `bypass`, connecte a un petit bouton en bas de l'Editor.

## Bypass

Le bypass est volontairement generique :

```txt
ParameterIDs::bypass
-> ParameterLayout.cpp
-> APVTS
-> ButtonAttachment dans PluginEditor
-> lecture dans PluginProcessor::processBlock
-> si actif, PluginDSP n'est pas appele
```

Comme le DSP du template est neutre, le bypass ne change pas encore le son. Il deviendra utile des qu'un traitement sera ajoute dans `PluginDSP`.

## Regles audio temps reel

Dans `processBlock()` :

- ne pas allouer de memoire
- ne pas utiliser `new` ou `malloc`
- ne pas utiliser de mutex/locks
- ne pas faire d'I/O disque
- ne pas faire d'appel bloquant
- eviter les logs
- ne pas creer de buffers temporaires a chaque bloc

Dans `PluginDSP::prepare()`:

- Préparer TOUTES les ressources
- Calculer les coefficients dépendant du sample rate (filtres, delays, etc.)
- Allouer et initialiser les buffers nécessaires (états de filtres, mémoires, etc.)

Dans `PluginDSP::reset()` :

- Réinitialiser l'état

## CMake

Le build passe par `CMakeLists.txt`.

Modifier seulement :
- le nom du projet
- le nom du produit
- la liste `SourceFiles` si de nouveaux fichiers sont ajoutes

Ne pas changer le systeme de build sauf necessite.

Commandes :

```bash
cmake -B build
cmake --build build
```

Targets utiles :

```bash
cmake --build build --target INSERT_PLUGIN_NAME_Standalone
cmake --build build --target INSERT_PLUGIN_NAME_VST3
```

## Style attendu

- rester simple et pedagogique
- garder le template neutre sauf demande explicite
- preferer des noms explicites
- utiliser `juce::` explicitement
- preferer `constexpr`, `enum class`, `std::unique_ptr`, `override`
- eviter macros inutiles, variables globales et abstractions prematurees
- commenter seulement les choix importants ou les contraintes temps reel

Avant de modifier, comprendre l'architecture existante et ajouter le code au bon endroit.
