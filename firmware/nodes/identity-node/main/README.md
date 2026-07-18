# Profile Images — Identity Node

Arquivos gerados para o M5Dial:

- resolução: 96 × 96 pixels;
- formato: RGB565 (`uint16_t`);
- fundo externo à máscara circular: preto;
- armazenamento atual: arrays embarcados em Flash;
- seleção centralizada em `ProfileImageManager`.

## Perfis

- `claudio`
- `herminio`
- `mariana`
- `student` (foto de Júnior)
- `unknown`

## Instalação

Copie:

- `profile_image_manager.h`
- `profile_image_manager.cpp`
- a pasta `profile_images/`

para a pasta `main/` do Identity Node.

O `main/CMakeLists.txt` deve incluir:

```cmake
SRCS
    "main.cpp"
    "profile_image_manager.cpp"
```

## Atenção ao layout

Os avatares têm 96 × 96 pixels. O layout atual do `main.cpp` foi originalmente
desenhado para um círculo menor. Antes do flash final, ajuste as posições dos
textos para evitar sobreposição.
