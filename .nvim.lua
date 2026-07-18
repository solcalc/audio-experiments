-- vim.keymap.set(mode, lhs, rhs, opts)
vim.keymap.set('n', '<leader>B', '<cmd>w<cr>:!gcc main.c --output main<cr>', { desc = 'Build file' })
vim.keymap.set('n', '<leader>R', ':!./main<cr>', { desc = 'Run file' })

