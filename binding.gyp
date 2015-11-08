{
  'targets': [{
    'target_name': 'setsockopt',
    'sources': ['main.cc'],
    'include_dirs': [
      '<!(node -e "require(\'nan\')")'
    ],
    'conditions': [
      ['OS=="win"',
        {
          'libraries': [
            'ws2_32.lib'
          ]
        }
      ]
    ]
  }]
}
