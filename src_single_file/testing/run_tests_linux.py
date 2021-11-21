import os, subprocess

test_files_path = 'test_files'
succeded_files_dir = os.path.join(test_files_path, 'succeded')
test_files = filter(lambda f : os.path.isfile(os.path.join(test_files_path, f)), os.listdir(test_files_path))

if not os.path.isdir(succeded_files_dir ):
    os.mkdir(succeded_files_dir)

command = './parsefile'
succeded_tests = []

for jsonfile in test_files:
    expected_return = 2
    if jsonfile[0]=='y':
        expected_return = 0
    elif jsonfile[0]=='n':
        expected_return = 1


    print('Testing \''+jsonfile+'\' ...', end=' ')
    argument = os.path.join(test_files_path, jsonfile)
    process = subprocess.Popen([command, argument], stdout=subprocess.PIPE)
    while True:
        output = process.stdout.readlines()
        return_code = process.poll()
        if return_code is not None:
            if return_code == expected_return or expected_return == 2:
                print('\t[OK]')
                succeded_tests.append(jsonfile)
            else:
                print('\t[FAIL]')
                #failed_tests.append(jsonfile)
            break

for succ_file in succeded_tests:
    from_dir = os.path.join(test_files_path, succ_file)
    to_dir = os.path.join(succeded_files_dir, succ_file)
    os.rename(from_dir, to_dir)