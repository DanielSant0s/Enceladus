{% if site_name %}
site_name: {{ site_name }}
{% else %}
site_name: Docs
{% endif %}

theme:
  name: material
  palette:
    # Dark Mode
    - scheme: slate
      toggle:
        icon: material/weather-sunny
        name: Dark mode
      primary: indigo
      accent: teal

    # Light Mode
    - scheme: default
      toggle:
        icon: material/weather-night
        name: Light mode
      primary: blue
      accent: deep orange
  font:
    text: Roboto
    code: Roboto Mono
  search: true
  features:
    - quick-links
    - navigation
    - search
    - toc
    - fullscreen
    - bookmarks
    - tabs
    - code
    - edit-on-github
    - language-selector
    - code
    - cover
    - footer
    - social-share
    - theme-switcher
nav:
  - Home: index.md
  {% if types %}
  - Types:
    {% for item in types -%}
    - {{ item.name }} : {{ item.file }}
    {% endfor %}
  {% endif %}
  {% if modules %}
  - Modules:
    {% for item in modules -%}
    - {{ item.name }} : {{ item.file }}
    {% endfor %}
  {% endif %}
  {% if globals %}
  - Globals:
    {% for item in globals -%}
    - {{ item.name }} : {{ item.file }}
    {% endfor %}
  {% endif %}
extra:
  footer: ""
